#include "local-node.h"
#include "../core/common.h"

namespace Epyx
{

    LocalNode::LocalNode(const std::string& name)
    :WorkerPool(1, true, name), id(), relay(NULL) {
    }

    std::ostream& operator<<(std::ostream& os, const LocalNode& node) {
        return os << node.id;
    }

    void LocalNode::attach(LocalRelay *rel) {
        EPYX_ASSERT(rel != NULL);
        this->id = rel->attachNode(this);
        this->relay = rel;
    }

    void LocalNode::send(const N2NP::NodeId& to, const N2NP::Packet& pkt) {
        EPYX_ASSERT(this->relay != NULL);

        // Allocate a new packet and forward it by the relay
        // (Note: packet is destroyed by Relay::treat())
        N2NP::Packet *real_pkt = new N2NP::Packet(pkt);
        real_pkt->from = this->id;
        real_pkt->to = to;
        relay->post(real_pkt);
    }

    void LocalNode::registerRecv(const std::string& method, ReceiveCb *cb, void* cbData) {
        ReceiveCbData cbEntry = {cb, cbData};
        recvCallbacksMutex.lock();
        recvCallbacks.insert(std::make_pair(method, cbEntry));
        recvCallbacksMutex.unlock();
    }

    void LocalNode::treat(N2NP::Packet *pkt) {
        EPYX_ASSERT(pkt != NULL);

        // Ensure I am the destination
        if (pkt->to != this->id) {
            log::error << "[Node " << *this << "] "
                << "Bad routing, I received a message for " << pkt->to << log::endl;
            return;
        }

        // Debug
        //log::debug << "[Node " << *this << "] Recv from " << pkt->from
        //    << " method " << pkt->method << log::endl;

        // Find the recv callback
        bool foundCallback = false;
        ReceiveCbData recvCbData;

        this->recvCallbacksMutex.lock();
        if (this->recvCallbacks.count(pkt->method)) {
            recvCbData = (*(this->recvCallbacks.find(pkt->method))).second;
            foundCallback = true;
        }
        this->recvCallbacksMutex.unlock();

        if (!foundCallback) {
            log::error << "[Node " << *this << "] Unknown method " << pkt->method << log::endl;
            return;
        }

        // Call the callback
        bool result = false;
        try {
            result = (*recvCbData.cb)(*this, *pkt, recvCbData.arg);
        } catch (Exception e) {
            log::error << "[Node " << *this << "] Exception " << e << log::endl;
            result = false;
        }
        if (!result) {
            log::error << "[Node " << *this << "] Problem with message"
                << " from " << pkt->from
                << " method " << pkt->method << log::endl;
        }
    }
}
