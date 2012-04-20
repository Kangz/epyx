#include "node.h"
#include "../core/common.h"
#include "../net/tcpsocket.h"

namespace Epyx
{
    namespace N2NP
    {

        Node::Node(const Address& addr)
        :NetSelectSocket(new TCPSocket(addr)), hasId(false) {
            this->send(NodeId("",addr),"ID",0,NULL);
        }

        bool Node::send(const NodeId& to, const std::string& method,
            unsigned long size, const char *data) {
            if(method != "ID")
                EPYX_ASSERT(hasId);

            // Send packet to the relay by default
            Packet n2npPkt(method, size, data);
            n2npPkt.from = nodeid;
            n2npPkt.to = to;

            //log::info << "Node " << nodeid << ": Send " << n2npPkt << log::endl;
            return n2npPkt.send(this->socket());
        }

        bool Node::send(const NodeId& to, const std::string& method,
            const GTTPacket& pkt) {
            char *data = NULL;
            unsigned long size = pkt.build(&data);
            if (data == NULL) {
                log::error << "N2NP node: Unable to build GTT packet to send" << log::endl;
                return false;
            }
            bool result = this->send(to, method, size, data);
            delete[] data;
            return result;
        }

        void Node::registerRecv(const PacketType& type, Node::ReceiveCb *cb, void* cbData) {
            ReceiveCbData cbEntry = {cb, cbData};
            recvCallbacksMutex.lock();
            recvCallbacks[type] = cbEntry;
            recvCallbacksMutex.unlock();
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

            // Find the recv callback
            bool foundCallback = false;
            ReceiveCbData recvCbData;

            this->recvCallbacksMutex.lock();
            if (this->recvCallbacks.count(pkt->type)) {
                recvCbData = (*(this->recvCallbacks.find(pkt->type))).second;
                foundCallback = true;
            }
            this->recvCallbacksMutex.unlock();

            if (!foundCallback) {
                log::error << "[Node " << nodeid << "] Unhandled method "
                    << pkt->type << log::endl;
                return;
            }

            // Call the callback
            bool result = false;
            try {
                result = (*recvCbData.cb)(*this, *pkt, recvCbData.arg);
            } catch (Exception e) {
                log::error << "[Node " << nodeid << "] Exception " << e << log::endl;
                result = false;
            }
            if (!result) {
                log::error << "[Node " << nodeid << "] Problem with packet "
                    << pkt << log::endl;
            }
        }
    }
}
