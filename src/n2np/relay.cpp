#include "relay.h"
#include "../net/tcpsocket.h"
#include <stdlib.h>

namespace Epyx
{
    namespace N2NP
    {

        Relay::Relay(const Address& addr)
        :WorkerPool(1, true, "Relay@" + addr.toString()),
        relayAddr(addr), relayId(addr) {
        }

        Relay::~Relay() {
            // Delete every Node information
            nodesMutex.lock();
            for (std::map<std::string, NodeInfo*>::iterator i = nodes.begin();
                i != nodes.end(); i++) {
                if (i->second != NULL) {
                    delete(i->second);
                    i->second = NULL;
                }
            }
            nodesMutex.unlock();
        }

        NodeId Relay::attachNode(Socket *sock) {
            EPYX_ASSERT(sock != NULL);

            // Build a random unique node name
            std::ostringstream nodeNameStream;
            nodeNameStream << sock->getAddress()
                << '-' << String::fromInt(rand() % 10000)
                << '-' << String::fromInt(rand() % 10000)
                << '-' << String::fromInt(rand() % 10000);
            const std::string nodeName = nodeNameStream.str();
            const NodeId nodeid(nodeName.c_str(), relayAddr);

            // Make node information
            NodeInfo *node = new NodeInfo();
            node->id = nodeid;
            node->sock = sock;

            // Insert node
            nodesMutex.lock();
            nodes[nodeName] = node;
            nodesMutex.unlock();

            log::info << "Relay: Attach node " << nodeid
                << " from " << sock->getAddress() << log::endl;

            // Send node ID to node
            Packet pkt(PacketType("ID"), 0, NULL);
            pkt.method = "ID";
            pkt.from = relayId;
            pkt.to = nodeid;
            pkt.send(*sock);

            // Return ID
            return nodeid;
        }

        bool Relay::detachNode(const NodeId& nodeid) {
            if (nodeid.getRelay() != relayAddr)
                return false;

            log::info << "Relay: Detach node " << nodeid << log::endl;

            nodesMutex.lock();
            nodes.erase(nodeid.getName());
            nodesMutex.unlock();
            return true;
        }

        const NodeId& Relay::getId() const {
            return relayId;
        }

        void Relay::treat(N2NP::Packet *pkt) {
            EPYX_ASSERT(pkt != NULL);
            // TODO: Send a message back to from node when an error happens

            // Send packet to another relay
            const Address toAddr = pkt->to.getRelay();
            if (toAddr != relayAddr) {
                TCPSocket toSock(toAddr);
                log::info << "Relay " << relayId << ": Transmit " << *pkt << log::endl;
                if (!pkt->send(toSock)) {
                    log::error << "[Relay " << relayId << "] Unable to transmit packet "
                        << *pkt << log::endl;
                    return;
                }
                // OK, packet was successfully transmitted
                return;
            }

            // Packet is for one of my nodes. Let's find it in the map !
            const std::string toName = pkt->to.getName();
            NodeInfo *node = NULL;
            this->nodesMutex.lock();
            if (this->nodes.count(toName))
                node = (*(this->nodes.find(toName))).second;
            this->nodesMutex.unlock();

            if (node == NULL) {
                log::debug << "[Relay " << relayId << "] Destination not found: "
                    << *pkt << log::endl;
                return;
            }

            // Send packet
            log::info << "Relay " << relayId << ": Send " << *pkt << log::endl;
            EPYX_ASSERT(node->sock != NULL);
            if (!pkt->send(*(node->sock))) {
                log::debug << "[Relay " << relayId << "] Unable to send packet "
                    << *pkt << log::endl;
                return;
            }
        }
    }
}
