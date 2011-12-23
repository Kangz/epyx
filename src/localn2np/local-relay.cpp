#include "local-relay.h"
#include "local-node.h"
#include "../core/exception.h"
#include <sstream>
#include <iostream>

namespace Epyx
{
    /**
     * Relay listening thread
     */
    static void LocalRelayRun(void *arg)
    {
        if (!arg)
            std::cerr << "[LocalRelayRun] Err: arg = NULL\n";
        else {
            LocalRelay *self = (LocalRelay*)arg;
            try {
                self->runLoop();
            } catch (Exception e) {
                std::cerr << e;
            }
        }
    }

    LocalRelay::LocalRelay(Address addr_)
        :addr(addr_), id(addr_), lastNodeId(0),
        runThread(LocalRelayRun, this)
    {
    }

    std::ostream& operator<<(std::ostream& os, const LocalRelay& relay)
    {
        return os << relay.id;
    }

    std::ostream& operator<<(std::ostream& os, const LocalRelay *relay)
    {
        if (!relay)
            return os << "(null)";
        else
            return os << (*relay);
    }

    N2npNodeId LocalRelay::attachNode(LocalNode *node)
    {
        std::string nodeName;

        // Make this thread-safe
        this->nodesMutex.lock();
        try {
            // Compute a new Unique ID
            std::ostringstream nodeNameStream;
            nodeNameStream << (++this->lastNodeId);
            nodeName = nodeNameStream.str();
        } catch (Exception e) {
            this->nodesMutex.unlock();
            throw e;
        }
        this->nodesMutex.unlock();

        N2npNodeId nodeId(nodeName.c_str(), this->addr);

        // This may not happen, as this is an auto-increment index
        if (this->nodes.count(nodeName))
            throw FailException("LocalRelay", "internal node counter error");

        this->nodesMutex.lock();
        this->nodes.insert(make_pair(nodeName, node));
        this->nodesMutex.unlock();

        return nodeId;
    }

    void LocalRelay::post(const N2npPacket& pkt)
    {
        this->packetQueueMutex.lock();
        // Here deeply copy the packet
        this->packetQueue.push_back(pkt);
        this->packetQueueMutex.unlock();
    }

    void LocalRelay::runLoop()
    {
        std::cout << "[Relay " << this << "] Running\n";
        while (true) {
            while (!this->packetQueue.empty()) {
                // Pop front packet
                this->packetQueueMutex.lock();
                N2npPacket pkt = this->packetQueue.front();
                this->packetQueue.pop_front();
                this->packetQueueMutex.unlock();

                // Get the node name
                std::string nodeName = pkt.to.getName();

                // Find the node
                LocalNode *node = NULL;
                this->nodesMutex.lock();
                if (this->nodes.count(nodeName))
                    node = (*(this->nodes.find(nodeName))).second;
                this->nodesMutex.unlock();

                // Send to the node if found
                if (node == NULL) {
                    std::cout << "[Relay " << this << "] Destination not found: "
                        << pkt.to << '\n';
                    continue;
                }
                // Debug
                //std::cout << "[Relay " << this << "] Send from " << pkt.from
                //    << " to " << node << '\n';

                //Post
                node->post(pkt);

            }
            usleep(100);
        }
    }

    void LocalRelay::run()
    {
        this->runThread.run();
    }
}
