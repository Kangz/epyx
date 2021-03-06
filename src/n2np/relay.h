/*
 *   Copyright 2012 Epyx Team
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
/**
 * @file n2np/relay.h
 * @brief Implement N2NP Relay main functions
 */

#ifndef EPYX_N2NP_RELAY_H
#define EPYX_N2NP_RELAY_H

#include "../core/worker-pool.h"
#include "../core/timeout.h"
#include "../net/sockaddress.h"
#include "../net/socket.h"
#include "../net/netselecttcpserver.h"
#include "packet.h"
#include "relaysocket.h"
#include <atomic>
#include <condition_variable>
#include <map>

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
            Relay(const SockAddress& addr);

            /**
             * @brief Attach a new node to the relay
             * @param sock socket to communicate with
             * @return new node ID
             * @throw Exception if there is a problem
             */
            NodeId attachNode(const std::shared_ptr<Socket>& sock);

            /**
             * @brief Detach a node
             * @param nodeid Node ID
             * @return true on success
             */
            bool detachNode(const NodeId& nodeid);

            /**
             * @brief Wait for all nodes to close connections
             * @param msec maximum number of milliseconds to wait
             * @return true if there are no more node
             */
            bool waitForAllDetach(int msec);

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
            void treat(std::unique_ptr<Packet> pkt);

        private:
            // Relay address and ID
            SockAddress relayAddr;
            NodeId relayId;

            // Structure to store information about a node
            typedef struct NodeInfo
            {
                NodeId id;
                // This socket as it is shared with RelaySocket (and NetSelect)
                std::shared_ptr<Socket> sock;
                NodeInfo(const NodeId& id, const std::shared_ptr<Socket>& sock);
                ~NodeInfo();
            } NodeInfo;

            // Map of known nodes (with its Mutex)
            std::mutex nodesMutex;
            std::condition_variable nodesCond;
            std::map<std::string, std::unique_ptr<NodeInfo> > nodes;

            // use a counter to attribute nodes
            std::atomic<unsigned long> nodeNextId;
        };
    }
}

#endif /* EPYX_N2NP_RELAY_H */

