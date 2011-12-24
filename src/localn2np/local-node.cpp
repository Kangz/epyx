#include "local-node.h"
#include "../core/exception.h"

namespace Epyx
{
    /**
     * Node listening thread
     */
    static void LocalNodeRun(void *arg)
    {
        if (!arg)
            std::cerr << "[LocalNodeRun] Err: arg = NULL\n";
        else {
            LocalNode *self = (LocalNode*)arg;
            try {
                self->runLoop();
            } catch (Exception e) {
                std::cerr << e;
            }
        }
    }

    LocalNode::LocalNode()
        :id(), relay(NULL), runThread(LocalNodeRun, this)
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
        if (relay_ == NULL)
            throw FailException("LocalNode", "trying to attach null");
        this->id = relay_->attachNode(this);
        this->relay = relay_;
    }

    void LocalNode::send(const N2npNodeId& to, const N2npPacket& pkt_)
    {
        if (this->relay == NULL)
            throw FailException("LocalNode", "sending data to nowhere");

        // Copy packet and forward it by the relay
        N2npPacket pkt(pkt_);
        pkt.from = this->id;
        pkt.to = to;
        this->relay->post(pkt);
    }

    void LocalNode::post(const N2npPacket& pkt)
    {
        this->packetQueueMutex.lock();
        // Here deeply copy the packet
        this->packetQueue.push_back(pkt);
        this->packetQueueMutex.unlock();
    }

    void LocalNode::runLoop()
    {
        std::cout << "[Node " << this << "] Running\n";
        while (true) {
            while (!this->packetQueue.empty()) {
                // Pop front packet
                this->packetQueueMutex.lock();
                N2npPacket pkt = this->packetQueue.front();
                this->packetQueue.pop_front();
                this->packetQueueMutex.unlock();

                // Ensure I am the destination
                if (pkt.to != this->id) {
                    std::cout << "[Node " << this << "] "
                        << "Bad routing, I received a message for " << pkt.to << '\n';
                    continue;
                }

                // Debug
                //std::cout << "[Node " << this << "] Recv from " << pkt.from
                //    << " type " << pkt.type << '\n';

                // Find the recv callback
                bool foundCallback = false;
                ReceiveCbData recvCbData;

                this->recvCallbacksMutex.lock();
                if (this->recvCallbacks.count(pkt.type)){
                    recvCbData = (*(this->recvCallbacks.find(pkt.type))).second;
                    foundCallback = true;
                }
                this->recvCallbacksMutex.unlock();

                if (!foundCallback) {
                    std::cout << "[Node " << this << "] Unknown type " << pkt.type << '\n';
                    continue;
                }

                // Call the callback
                bool result = false;
                try {
                    result = (*recvCbData.cb)(*this, pkt, recvCbData.arg);
                } catch (Exception e) {
                    std::cerr << "[Node " << this << "] Exception " << e;
                    result = false;
                }
                if (!result)
                    std::cout << "[Node " << this << "] Problem with message"
                        << " from " << pkt.from
                        << " type " << pkt.type << '\n';
            }
            usleep(100);
        }
    }

    void LocalNode::run()
    {
        this->runThread.run();
    }

    void LocalNode::registerRecv(const N2npPacketType& type, ReceiveCb *cb, void* cbData)
    {
        ReceiveCbData cbEntry = {cb, cbData};
        this->recvCallbacksMutex.lock();
        this->recvCallbacks.insert(std::make_pair(type, cbEntry));
        this->recvCallbacksMutex.unlock();
    }
}
