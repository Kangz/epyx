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
 * @file local-relay.h
 * @brief Local Relay management
 *
 * Simulation of N2NP
 */

#ifndef EPYX_LOCAL_RELAY_H
#define EPYX_LOCAL_RELAY_H

#include "../n2np/packet.h"
#include "../net/sockaddress.h"
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
        LocalRelay(const SockAddress& addr);

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
        SockAddress addr;
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
