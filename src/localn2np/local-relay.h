/**
 * @file local-relay.h
 * @brief Local Relay management
 *
 * Simulation of N2NP
 */

#ifndef EPYX_LOCAL_RELAY_H
#define EPYX_LOCAL_RELAY_H

#include "../n2np/packet.h"
#include "../net/address.h"
#include "../core/mutex.h"
#include "../core/worker-pool.h"
#include <ostream>
#include <map>

namespace Epyx
{
    class LocalNode;

    /**
     * @class LocalRelay
     * @brief Implement local N2NP relay simulation with a worker pool
     */
    class LocalRelay : public WorkerPool<N2NP::Packet>
    {
    public:
        /**
         * @brief Constructor
         * @param addr Relay address
         */
        LocalRelay(const Address& addr);

        /**
         * @brief Display a node in an output stream
         * @param os Output stream
         * @param relay
         */
        friend std::ostream& operator<<(std::ostream& os, const LocalRelay& relay);

        /**
         * @brief A node called this function to be attached with this relay
         * @param node new node
         * @return new node ID
         */
        N2NP::NodeId attachNode(LocalNode *node);

    protected:
        /**
         * @brief Treat a N2NP packet
         * @param pkt the packet to be processed
         */
        void treat(N2NP::Packet *pkt);

    private:
        Address addr;
        N2NP::NodeId id;

        std::map<std::string, LocalNode*> nodes;
        unsigned int lastNodeId;
        Mutex nodesMutex;

        // Disable copy
        LocalRelay(const LocalRelay&);
        const LocalRelay& operator=(const LocalRelay&);
    };
}

#endif /* EPYX_LOCAL_RELAY_H */
