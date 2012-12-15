#include "node.h"
#include "../core/common.h"
#include "../net/tcpsocket.h"
#include "../core/string.h"
#include <cstring>

#ifndef EPYX_N2NP_NODE_DEBUG
/**
 * @brief N2NP::Node debug
 */
#define EPYX_N2NP_NODE_DEBUG 0
#endif

namespace Epyx
{
    namespace N2NP
    {

        Node::Node(const SockAddress& addr)
        :NetSelectSocket(new TCPSocket(addr)), hasId(false), curId(0) {
            this->send(NodeId("", addr), "ID", byte_str());
        }

        bool Node::send(const NodeId& to, const std::string& method,
            const byte_str& data, bool store) {
            if (method != "ID")
                EPYX_ASSERT(hasId);

            // Send packet to the relay by default
            std::shared_ptr<Packet> n2npPkt(new Packet(method, data));
            n2npPkt->from = nodeid;
            n2npPkt->to = to;
            n2npPkt->pktID = std::atomic_fetch_add(&curId, 1UL);

            //log::info << "Node " << nodeid << ": Send " << n2npPkt << log::endl;
            if (store) {
                if (sentMap.get(n2npPkt->pktID, NULL) != NULL)
                    throw FailException("N2NP::Node", "Duplicate packet ID while sending");
                sentMap.set(n2npPkt->pktID, n2npPkt);
#if EPYX_N2NP_NODE_DEBUG
                if (this->hasId)
                    log::debug << "Node " << this->getId() << " has a map of "
                    << sentMap.size() << " packets." << log::endl;
#endif
            }

            // Statistics
            unsigned int weight = (data.size() > 1000) ? 1000 : data.size();
            unsigned int cnt = mruNodeIds.getAndLock(to, 0);
            mruNodeIds.setLocked(to, cnt + weight);
            mruNodeIds.endUnlock();

            //We need to select a socket to send to.
            Socket *selected = NULL;
            {
                std::lock_guard<std::mutex> lock(directSocketsMutex);
                auto iter = directSockets.find(to);
                if (iter != directSockets.end()) {
                    // Direct connection to the node
                    selected = iter->second.get();
                } else {
                    iter = directSockets.find(to.getRelay());
                    if (iter != directSockets.end()) {
                        // Use destination relay
                        selected = iter->second.get();
                    } else {
                        // Use my relay
                        selected = this->socket().get();
                    }
                }
            }
            EPYX_ASSERT(selected != NULL);
            return selected->sendBytes(n2npPkt->build());
        }

        void Node::sendAck(const Packet& pkt) {
#if EPYX_N2NP_NODE_DEBUG
            log::debug << "Acknowledging a packet !" << log::endl;
#endif
            std::string s = String::fromUnsignedLong(pkt.pktID);
            this->send(pkt.from, "ACK", string2bytes_c(s), false);
        }

        void Node::sendErr(const Packet& pkt) {
#if EPYX_N2NP_NODE_DEBUG
            log::debug << "Erroring a packet !" << log::endl;
#endif
            std::string s = String::fromUnsignedLong(pkt.pktID);
            this->send(pkt.from, "ERR", string2bytes_c(s), false);
        }

        bool Node::send(const NodeId& to, const std::string& method,
            const GTTPacket& pkt) {
            const byte_str payload = pkt.build();
            if (payload.empty()) {
                log::error << "N2NP node: Unable to build GTT packet to send" << log::endl;
                return false;
            }
            return this->send(to, method, payload);
        }

        void Node::addModule(const std::string& method, const std::shared_ptr<Module>& m) {
            EPYX_ASSERT(m);
            std::lock_guard<std::mutex> lock(modulesMutex);
            if (modules.find(method) != modules.end())
                throw FailException("N2NP::Node", "Cannot add a module to an already bound key");
            modules[method] = m;
        }

        void Node::offerDirectConn(const NodeId& recipient, std::unique_ptr<Socket>&& socket) {
            std::lock_guard<std::mutex> lock(directSocketsMutex);
            directSockets[recipient].swap(socket);
        }

        const NodeId& Node::getId() const {
            EPYX_ASSERT(hasId);
            return nodeid;
        }

        bool Node::isReady() const {
            return hasId;
        }

        bool Node::waitReady(const Timeout& timeout) const {
            while (!hasId && !timeout.hasExpired()) {
                usleep(1);
            }
            return hasId;
        }

        const SockAddress& Node::getNodeAddress() const {
            return nodeAddressFromRelay;
        }

        void Node::askForDirectConnectionIds(std::stack<NodeId>& stackIds) {
            std::lock_guard<std::mutex> lock(directSocketsMutex);
            // mruNodeIds is a map NodeId => weight
            for (auto i = mruNodeIds.beginLock(); mruNodeIds.isEnd(i); i++) {
                if (directSockets.find(i->first) == directSockets.end()) {
                    stackIds.push(i->first);
                    i->second = 0;
                }
            }
            mruNodeIds.endUnlock();
        }

        void Node::eat(const byte_str& data) {
            gttparser.eat(data);
            std::unique_ptr<GTTPacket> gttpkt;
            // Loop over each packet
            while ( (gttpkt = gttparser.getPacket()) ) {
                // Build a N2NP packet and post it to the Relay
                std::unique_ptr<Packet> n2nppkt(new Packet(*gttpkt));
                this->treat(n2nppkt);
            }
            std::string error;
            if (gttparser.getError(error)) {
                log::error << "N2NP Node: GTT Parser error for node " << nodeid
                    << ". Resetting parser." << log::endl;
                gttparser.reset();
            }
        }

        void Node::treat(const std::unique_ptr<Packet>& pkt) {
            EPYX_ASSERT(pkt);

            // Set node ID
            if (!hasId) {
                if (pkt->method != "ID") {
                    log::error << "[Node ?] "
                        << "Bad first message " << *pkt << log::endl;
                    return;
                }
                nodeid = pkt->to;
                hasId = true;
                //log::info << "Node " << nodeid << ": Received ID " << *pkt << log::endl;

                // Decode data
                LineParser parser;
                parser.push(pkt->data);
                parser.push(String::crlf_bytes);
                std::string line;
                while (parser.popLine(line)) {
                    line = String::trim(line);
                    if (line.empty())
                        continue;
                    size_t commaPos = line.find(':');
                    if (commaPos == std::string::npos)
                        continue;
                    std::string name = line.substr(0, commaPos);
                    name = String::toLower(String::trim(name));
                    std::string value = line.substr(commaPos + 1);
                    value = String::trim(value);
                    if (name == "ip") {
                        // Set IP address as seen
                        nodeAddressFromRelay = SockAddress(value);
                    }
                }
                return;
            }

            // Ensure I am the destination
            if (pkt->to != nodeid) {
                log::error << "[Node " << nodeid << "] "
                    << "Bad routing, I received a message for " << pkt->to << log::endl;
                return;
            }

            //log::info << "Node " << nodeid << ": Recv " << *pkt << log::endl;

            //Special treatment for internal method ERR and ACK
            if (pkt->method == "ACK") {
                unsigned long idAcked = String::toULong(bytes2string_c(pkt->data));
                if (sentMap.unset(idAcked)) {
#if EPYX_N2NP_NODE_DEBUG
                    log::debug << "Succesfully erased an acked packed" << log::endl;
#endif
                }
                return;
            } else if (pkt->method == "ERR") {
                return;
            }

            // Statistics
            size_t weight = (pkt->data.size() > 1000) ? 1000 : pkt->data.size();
            unsigned int cnt = mruNodeIds.getAndLock(pkt->from, 0);
            mruNodeIds.setLocked(pkt->from, cnt + weight);
            mruNodeIds.endUnlock();

            // Find the relevant module
            {
                std::lock_guard<std::mutex> lock(modulesMutex);
                auto moduleToCall = modules.find(pkt->method);
                if (moduleToCall == modules.end() || !moduleToCall->second) {
                    log::error << "[Node " << nodeid << "] Unhandled method "
                        << pkt->method << log::endl;
                    this->sendErr(*pkt);
                    return;
                }

                // Call the module
                try {
                    moduleToCall->second->fromN2NP(*this, pkt->from, pkt->data);
                } catch (const MinorException& e) {
                    log::error << "[Node " << nodeid << "] " << e << log::endl;
                }
            }

            //Everything went right, let's acknowledge the packet.
            this->sendAck(*pkt);
        }
    }
}
