#include "relay.h"
#include "../net/tcpsocket.h"
#include "../core/timeout.h"
#include <stdlib.h>

namespace Epyx
{
    namespace N2NP
    {

        Relay::Relay(const Address& addr)
        :WorkerPool(1, true, "Relay@" + addr.toString()),
        relayAddr(addr), relayId(addr), nodeNextId(1) {
            log::info << "Relay: Start " << relayId << log::endl;
        }

        Relay::~Relay() {
            this->detachAllNodes();
        }

        NodeId Relay::attachNode(Socket *sock) {
            EPYX_ASSERT(sock != NULL);

            // Build a random unique node name
            std::ostringstream nodeNameStream;
            nodeNameStream << String::fromInt(nodeNextId.getIncrement())
                << '-' << sock->getAddress();
            const std::string nodeName = nodeNameStream.str();
            const NodeId nodeid(nodeName.c_str(), relayAddr);

            // Make node information
            NodeInfo *node = new NodeInfo(nodeid, sock);

            // Insert node
            nodes.set(nodeName, node);

            log::info << "Relay: Attach node " << nodeid
                << " from " << sock->getAddress() << log::endl;

            // Send node ID to node
            Packet pkt("ID", 0, NULL);
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

            NodeInfo *node = nodes.getUnset(nodeid.getName(), NULL);
            if (node != NULL) {
                delete node;
            }
            return true;
        }

        bool Relay::waitForAllDetach(const Timeout& timeout) {
            while (!timeout.hasExpired()) {
                if (nodes.empty())
                    return true;
            }
            return false;
        }

        void Relay::detachAllNodes() {
            // Delete every Node information
            for (atom::Map<std::string, NodeInfo*>::iterator i = nodes.beginLock();
                !nodes.isEnd(i); i++) {
                if (i->second != NULL) {
                    delete(i->second);
                    i->second = NULL;
                }
            }
            nodes.endUnlock();
        }

        const NodeId& Relay::getId() const {
            return relayId;
        }

        void Relay::treat(Packet *pkt) {
            EPYX_ASSERT(pkt != NULL);
            // TODO: Send a message back to from node when an error happens

            // Send packet to another relay
            const Address toAddr = pkt->to.getRelay();
            if (toAddr != relayAddr) {
                TCPSocket toSock(toAddr);
                //log::info << "Relay " << relayId << ": Transmit " << *pkt << log::endl;
                if (!pkt->send(toSock)) {
                    log::error << "[Relay " << relayId << "] Unable to transmit packet "
                        << *pkt << log::endl;
                    return;
                }
                // OK, packet was successfully transmitted
                return;
            }

            // Packet is for one of my nodes. Let's find it in the map !
            NodeInfo *node = nodes.get(pkt->to.getName(), NULL);

            if (node == NULL) {
                log::error << "[Relay " << relayId << "] Destination not found: "
                    << *pkt << log::endl;
                return;
            }

            // Send packet
            //log::info << "Relay " << relayId << ": Send " << *pkt << log::endl;
            EPYX_ASSERT(node->sock != NULL);
            if (!pkt->send(*(node->sock))) {
                log::error << "[Relay " << relayId << "] Unable to send packet "
                    << *pkt << log::endl;
                return;
            }
        }

        Relay::NodeInfo::NodeInfo(const NodeId& id, Socket *sock)
        :id(id), sock(sock) {
        }

        Relay::NodeInfo::~NodeInfo() {
            if (sock) {
                // NEVER DELETE THIS SOCKET, but you may close it
                sock->close();
                sock = NULL;
            }
        }
    }
}
