/**
 * @file relay.h
 * @brief Implement N2NP Relay main functions
 */

#ifndef EPYX_N2NP_RELAY_H
#define EPYX_N2NP_RELAY_H

#include "../core/mutex.h"
#include "../core/worker-pool.h"
#include "../net/address.h"
#include "../net/socket.h"
#include "../net/netselecttcpserver.h"
#include "packet.h"
#include "relaysocket.h"

namespace Epyx
{
    namespace N2NP
    {
        /**
         * @class Relay
         * @brief N2NP Relay implementation
         */
        class Relay : public WorkerPool<Packet>
        {
        public:
            /**
             * @brief Create a new relay for a listen address
             * @param addr
             */
            Relay(const Address& addr);

            ~Relay();

            /**
             * @brief Attach a new node to the relay
             * @param sock socket to communicate with
             * @return new node ID
             * @throw Exception if there is a problem
             */
            NodeId attachNode(Socket *sock);

            /**
             * @brief Detach a node
             * @param nodeid Node ID
             * @return true on success
             */
            bool detachNode(const NodeId& nodeid);

            /**
             * @brief Wait for all nodes to close connections
             * @param timeout maximum number of seconds to wait
             * @return true if there are no more node
             */
            bool waitForAllDetach(int timeout);

            /**
             * @brief Force to dettach all nodes
             */
            void detachAllNodes();

            /**
             * @brief Get relay ID
             * @return this->relayId
             */
            const NodeId& getId() const;

        protected:
            /**
             * @brief Treat a N2NP packet
             * @param pkt the packet to be processed
             */
            void treat(Packet *pkt);

        private:
            // Relay address and ID
            Address relayAddr;
            NodeId relayId;

            // Structure to store information about a node
            typedef struct NodeInfo
            {
                NodeId id;
                // Important: NEVER delete this socket as it is managed by
                // RelaySocket (and NetSelect)
                Socket *sock;
                NodeInfo(const NodeId& id, Socket * sock);
                ~NodeInfo();
            } NodeInfo;

            // Map of known nodes (with its Mutex)
            std::map<std::string, NodeInfo*> nodes;
            Mutex nodesMutex;

            // Disable copy
            Relay(const Relay&);
            const Relay& operator=(const Relay&);
        };
    }
}

#endif /* EPYX_N2NP_RELAY_H */

