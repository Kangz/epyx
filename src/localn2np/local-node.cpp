#include "local-node.h"
#include "../core/common.h"

namespace Epyx
{
    LocalNode::LocalNode()
        :id(), relay(NULL)
    {
    }

    std::ostream& operator<<(std::ostream& os, const LocalNode& node)
    {
        return os << node.id;
    }

    std::ostream& operator<<(std::ostream& os, const LocalNode *node)
    {
        if (!node)
            return os << "(null)";
        else
            return os << (*node);
    }

    void LocalNode::attach(LocalRelay *relay_)
    {
        EPYX_ASSERT(relay_ != NULL);
        this->id = relay_->attachNode(this);
        this->relay = relay_;
    }

    void LocalNode::send(const N2npNodeId& to, const N2npPacket& pkt_)
    {
        EPYX_ASSERT(this->relay != NULL);

        // Copy packet and forward it by the relay
        N2npPacket pkt(pkt_);
        pkt.from = this->id;
        pkt.to = to;
        this->relay->post(pkt);
    }
    
    void LocalNode::registerRecv(const N2npPacketType& type, ReceiveCb *cb, void* cbData)
    {
        ReceiveCbData cbEntry = {cb, cbData};
        this->recvCallbacksMutex.lock();
        this->recvCallbacks.insert(std::make_pair(type, cbEntry));
        this->recvCallbacksMutex.unlock();
    }

    void LocalNode::run()
    {
        log::debug << "[Node " << this << "] Running" << log::endl;
        while (true) {
            N2npPacket *pkt = this->packetQueue.pop();
            // Null packet means the queue is closed
            if (pkt == NULL)
                return;
            
            // Ensure I am the destination
            if (pkt->to != this->id) {
                log::error << "[Node " << this << "] "
                    << "Bad routing, I received a message for " << pkt->to << log::endl;
                continue;
            }
            
            // Debug
            //log::debug << "[Node " << this << "] Recv from " << pkt->from
            //    << " type " << pkt->type << log::endl;

            // Find the recv callback
            bool foundCallback = false;
            ReceiveCbData recvCbData;

            this->recvCallbacksMutex.lock();
            if (this->recvCallbacks.count(pkt->type)){
                recvCbData = (*(this->recvCallbacks.find(pkt->type))).second;
                foundCallback = true;
            }
            this->recvCallbacksMutex.unlock();

            if (!foundCallback) {
                log::error << "[Node " << this << "] Unknown type " << pkt->type << log::endl;
                continue;
            }
            
            // Call the callback
            bool result = false;
            try {
                result = (*recvCbData.cb)(*this, *pkt, recvCbData.arg);
            } catch (Exception e) {
                log::error << "[Node " << this << "] Exception " << e << log::endl;
                result = false;
            }
            if (!result) {
                log::error << "[Node " << this << "] Problem with message"
                    << " from " << pkt->from
                    << " type " << pkt->type << log::endl;
            }
        }
    }
}
