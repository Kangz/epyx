#include "node.h"
#include "../core/common.h"
#include "../net/tcpsocket.h"

namespace Epyx
{
    namespace N2NP
    {

        Node::Node(const Address& addr)
        :NetSelectSocket(new TCPSocket(addr)), hasId(false), curId(0) {
            this->send(NodeId("",addr),"ID",NULL,0);
        }

        bool Node::send(const NodeId& to, const std::string& method,
            const char *data, unsigned long size) {
            if(method != "ID")
                EPYX_ASSERT(hasId);

            // Send packet to the relay by default
            Packet *n2npPkt = new Packet(method, size, data);
            n2npPkt->from = nodeid;
            n2npPkt->to = to;
            n2npPkt->pktID = curId.getIncrement();

            //log::info << "Node " << nodeid << ": Send " << n2npPkt << log::endl;
            if(sentMap.count(n2npPkt->pktID) == 1)
                throw new FailException("N2NP::Node", "Duplicate packet ID while sending");
            sentMap[n2npPkt->pktID] = n2npPkt;
            if(this->hasId)
                log::debug << "Node " << this->getId() << " has a map of " << sentMap.size() << " packets." << log::endl;
            return n2npPkt->send(this->socket());
        }

        bool Node::send(const NodeId& to, const std::string& method,
            const GTTPacket& pkt) {
            char *data = NULL;
            unsigned long size = pkt.build(&data);
            if (data == NULL) {
                log::error << "N2NP node: Unable to build GTT packet to send" << log::endl;
                return false;
            }
            bool result = this->send(to, method, data, size);
            delete[] data;
            return result;
        }

        void Node::addModule(std::string method, Module *m) {
            modulesMutex.lock();
            if(modules.count(method) > 0) {
            modulesMutex.unlock();
            throw new FailException("N2NP::Node", "Cannot add a module to an already bound key");
            }
            modules[method] = m;
            modulesMutex.unlock();
        }

        const NodeId& Node::getId() const {
            EPYX_ASSERT(hasId);
            return nodeid;
        }

        bool Node::isReady() const {
            return hasId;
        }

        void Node::eat(const char *data, long size) {
            gttparser.eat(data, size);
            GTTPacket *gttpkt = NULL;
            // Loop over each packet
            while ((gttpkt = gttparser.getPacket()) != NULL) {
                // Build a N2NP packet and post it to the Relay
                Packet *n2nppkt = new Packet(*gttpkt);
                this->treat(n2nppkt);
                delete n2nppkt;
            }
            std::string error;
            if (gttparser.getError(error)) {
                log::error << "N2NP Node: GTT Parser error for node " << nodeid
                    << ". Resetting parser." << log::endl;
                gttparser.reset();
            }
        }

        void Node::treat(Packet *pkt) {
            EPYX_ASSERT(pkt != NULL);

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
                return;
            }

            // Ensure I am the destination
            if (pkt->to != nodeid) {
                log::error << "[Node " << nodeid << "] "
                    << "Bad routing, I received a message for " << pkt->to << log::endl;
                return;
            }

            //log::info << "Node " << nodeid << ": Recv " << *pkt << log::endl;

            // Find the relevant module
            bool foundModule = false;
            Module* moduleToCall;

            this->modulesMutex.lock();
            if (this->modules.count(pkt->method)) {
                moduleToCall = this->modules[pkt->method];
                foundModule = true;
            }
            this->modulesMutex.unlock();

            if (!foundModule) {
                log::error << "[Node " << nodeid << "] Unhandled method "
                    << pkt->method << log::endl;
                return;
            }

            // Call the module
            try {
                moduleToCall->fromN2NP(*this, pkt->from, pkt->data, pkt->size);
            } catch (MinorException e) {
                log::error << "[Node " << nodeid << "] Exception " << e << log::endl;
            }
        }
    }
}
