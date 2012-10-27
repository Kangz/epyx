#include "relay.h"
#include "../net/tcpsocket.h"
#include "../core/timeout.h"
#include <stdlib.h>

namespace Epyx
{
    namespace N2NP
    {

        Relay::Relay(const SockAddress& addr)
        :WorkerPool(1, "Relay@" + addr.toString()),
        relayAddr(addr), relayId(addr), nodeNextId(1) {
            log::info << "Relay: Start " << relayId << log::endl;
        }

        NodeId Relay::attachNode(const std::shared_ptr<Socket>& sock) {
            EPYX_ASSERT(sock);

            // Build a random unique node name
            std::ostringstream nodeNameStream;
            long number = nodeNextId.getIncrement();
            if (number == 1) {
                nodeNameStream << "self";
            } else {
                nodeNameStream << String::fromInt(nodeNextId.getIncrement())
                    << '-' << sock->getAddress();
            }
            const std::string nodeName = nodeNameStream.str();
            const NodeId nodeid(nodeName.c_str(), relayAddr);

            // Insert new node information
            {
                std::unique_ptr<NodeInfo> node(new NodeInfo(nodeid, sock));
                std::lock_guard<std::mutex> lock(nodesMutex);
                nodes[nodeName].swap(node);
            }

            log::info << "Relay: Attach node " << nodeid
                << " from " << sock->getAddress() << log::endl;

            // Send IP to node in data
            std::ostringstream dataStream;
            dataStream << "IP: " << sock->getAddress() << String::crlf;
            const std::string dataString = dataStream.str();
            Packet pkt("ID", string2bytes_c(dataString));
            pkt.from = relayId;
            pkt.to = nodeid;
            sock->sendBytes(pkt.build());

            // Return ID
            return nodeid;
        }

        bool Relay::detachNode(const NodeId& nodeid) {
            if (nodeid.getRelay() != relayAddr)
                return false;

            log::info << "Relay: Detach node " << nodeid << log::endl;
            std::lock_guard<std::mutex> lock(nodesMutex);
            nodes.erase(nodeid.getName());
            return true;
        }

        bool Relay::waitForAllDetach(const Timeout& timeout) {
            while (!timeout.hasExpired()) {
                std::lock_guard<std::mutex> lock(nodesMutex);
                if (nodes.empty())
                    return true;
            }
            return false;
        }

        void Relay::detachAllNodes() {
            // Delete every Node information
            std::lock_guard<std::mutex> lock(nodesMutex);
            nodes.clear();
        }

        const NodeId& Relay::getId() const {
            return relayId;
        }

        void Relay::treat(std::unique_ptr<Packet> pkt) {
            EPYX_ASSERT(pkt);
            // TODO: Send a message back to from node when an error happens

            // Send packet to another relay
            const SockAddress toAddr = pkt->to.getRelay();
            if (toAddr != relayAddr) {
                TCPSocket toSock(toAddr);
                //log::info << "Relay " << relayId << ": Transmit " << *pkt << log::endl;
                if (!toSock.sendBytes(pkt->build())) {
                    log::error << "[Relay " << relayId << "] Unable to transmit packet "
                        << *pkt << log::endl;
                    return;
                }
                // OK, packet was successfully transmitted
                return;
            }

            // Packet is for one of my nodes. Let's find it in the map !
            std::lock_guard<std::mutex> lock(nodesMutex);
            // node is an iterator on a map
            auto nodeit = nodes.find(pkt->to.getName());
            if (nodeit == nodes.end()) {
                log::error << "[Relay " << relayId << "] Destination not found: "
                    << *pkt << log::endl;
                return;
            }

            // Send packet
            //log::info << "Relay " << relayId << ": Send " << *pkt << log::endl;
            if (!nodeit->second->sock->sendBytes(pkt->build())) {
                log::error << "[Relay " << relayId << "] Unable to send packet "
                    << *pkt << log::endl;
                return;
            }
        }

        Relay::NodeInfo::NodeInfo(const NodeId& id, const std::shared_ptr<Socket>& sock)
        :id(id), sock(sock) {
        }

        Relay::NodeInfo::~NodeInfo() {
            // Close the socket
            if (sock) {
                sock->close();
            }
        }
    }
}
