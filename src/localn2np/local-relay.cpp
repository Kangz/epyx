#include "local-relay.h"
#include "../core/common.h"
#include "local-node.h"
#include <sstream>
#include <iostream>

namespace Epyx
{
    LocalRelay::LocalRelay(Address addr_)
        :addr(addr_), id(addr_), lastNodeId(0)
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
        EPYX_ASSERT(this->nodes.count(nodeName) == 0);

        this->nodesMutex.lock();
        this->nodes.insert(make_pair(nodeName, node));
        this->nodesMutex.unlock();

        return nodeId;
    }

    void LocalRelay::run()
    {
        log::debug << "[Relay " << this << "] Running" << log::endl;
        while (true) {
            N2npPacket *pkt =  this->packetQueue.pop();
            // Null packet means the queue is closed
            if (pkt == NULL)
                return;

            // Find the node
            std::string nodeName = pkt->to.getName();
            LocalNode *node = NULL;
            this->nodesMutex.lock();
            if (this->nodes.count(nodeName))
                node = (*(this->nodes.find(nodeName))).second;
            this->nodesMutex.unlock();

            // Send to the node if found
            if (node == NULL) {
                log::debug << "[Relay " << this << "] Destination not found: "
                    << pkt->to << log::endl;
                continue;
            }
            // Debug
            //log::debug << "[Relay " << this << "] Send from " << pkt.from
            //    << " to " << node << log::debug ;

            //Post
            node->post(*pkt);
        }
    }
}
