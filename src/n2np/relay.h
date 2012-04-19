/**
 * @file relay.h
 * @brief Implement N2NP Relay main functions
 */

#ifndef EPYX_N2NP_RELAY_H
#define    EPYX_N2NP_RELAY_H

#include "../core/mutex.h"
#include "../core/worker-pool.h"
#include "../net/address.h"
#include "../net/socket.h"
#include "packet.h"

namespace Epyx
{
    namespace N2NP
    {
        /**
         * @class Relay
         * @brief N2NP Relay implementation
         */
        class Relay : public WorkerPool<N2NP::Packet>
        {
        public:
            /**
             * @brief Create a new relay for a listen address
             * @param addr
             */
            Relay(const Address& addr);

        protected:
            /**
             * @brief Treat a N2NP packet
             * @param pkt the packet to be processed
             */
            void treat(N2NP::Packet *pkt);

        private:
            // Relay address and ID
            Address addr;
            NodeId id;

            // Structure to store information about a node
            typedef struct NodeInfo {
                NodeId id;
                Socket *sock;
            } NodeInfo;

            // Map of known nodes (with its Mutex)
            std::map<std::string, NodeInfo*> nodes;
            Mutex nodesMutex;
        };
    }
}

#endif /* EPYX_N2NP_RELAY_H */

