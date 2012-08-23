#include "local-relay.h"
#include "../core/common.h"
#include "local-node.h"
#include <sstream>
#include <iostream>

namespace Epyx
{

    LocalRelay::LocalRelay(const SockAddress& addr)
    :WorkerPool(1, true, "Relay" + addr.toString()), addr(addr), id(addr), lastNodeId(0) {
    }

    std::ostream& operator<<(std::ostream& os, const LocalRelay& relay) {
        return os << relay.id;
    }

    N2NP::NodeId LocalRelay::attachNode(LocalNode *node) {
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

        N2NP::NodeId nodeId(nodeName.c_str(), this->addr);

        // This may not happen, as this is an auto-increment index
        EPYX_ASSERT(this->nodes.count(nodeName) == 0);

        this->nodesMutex.lock();
        this->nodes.insert(make_pair(nodeName, node));
        this->nodesMutex.unlock();

        return nodeId;
    }

    void LocalRelay::treat(N2NP::Packet *pkt) {
        EPYX_ASSERT(pkt != NULL);

        // Find the node
        std::string nodeName = pkt->to.getName();
        LocalNode *node = NULL;
        this->nodesMutex.lock();
        if (this->nodes.count(nodeName))
            node = (*(this->nodes.find(nodeName))).second;
        this->nodesMutex.unlock();

        // Send to the node if found
        if (node == NULL) {
            log::debug << "[Relay " << *this << "] Destination not found: "
                << pkt->to << log::endl;
            return;
        }

        // Debug
        //log::debug << "[Relay " << *this << "] Send from " << pkt->from
        //    << " to " << node << log::debug ;

        //Post, with a copy because pkt is deleted after this function
        node->post(new N2NP::Packet(*pkt));
    }
}
