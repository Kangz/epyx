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
 * @file api.h
 * @brief Application Programming Interface of Epyx
 */

#ifndef EPYX_API_H
#define EPYX_API_H

#include "core/common.h"
#include "dht/node.h"
#include "net/netselect.h"
#include "net/netif.h"
#include "n2np/relay.h"
#include "n2np/node.h"
#include <boost/noncopyable.hpp>
#include <list>
#include <map>
#include <mutex>

namespace Epyx
{
    class API : private boost::noncopyable
    {
    public:
        /**
         * @brief Initialise all internal stuff
         * @param logflags
         * @param logfile
         */
        API(int logflags = log::CONSOLE, const std::string& logfile = "");

        /**
         * @brief End Epyx
         */
        ~API();

        /**
         * @brief Set the number of workers in NetSelect
         * @param numworkers
         */
        void setNetWorkers(int numworkers);

        /**
         * @brief Spawn a new relay (only one for the entire API)
         * @param addr IP address and TCP port
         * @param nbConn TCP maximal connection number to the relay
         */
        void spawnRelay(const SockAddress& addr, unsigned int nbConn = 50);

        /**
         * @brief Destroy specified relay
         * @param addr
         * @param msec number of msecs to wait for detach nodes
         */
        void destroyRelay(int msec);

        /**
         * @brief Add a new node
         * @param node a dynamicaly-allocated node
         *
         * @return identifier
         */
        int addNode(std::shared_ptr<N2NP::Node>& node);

        /**
         * @brief Create a new N2NP node and add it to the NetSelect
         * @param addr relay address
         * @param timeout milliseconds to wait until creation is alleged to have failed
         * @return the new node
         */
        std::shared_ptr<N2NP::Node> spawnN2NPNode(const SockAddress& addr, int timeout = 5000);

        /**
         * @brief Create a new DHT node on an existing NNP node
         * @param id DHT ID
         * @param name thread name
         * @param node N2NP node
         * @return shared pointer to a new DHT::Node
         */
        std::shared_ptr<DHT::Node> createDHTNode(const std::string& name,
                std::shared_ptr<N2NP::Node> node);

        /**
         * @brief Kill a node
         * @param nodeId
         */
        void destroyNode(int nodeId);

        /**
         * @brief Kill every nodes
         */
        void destroyAllNodes();

        /**
         * @brief Wait for the NetSelect to terminate
         */
        void waitNet();

    private:
        // Main mutex, for netsel and relay, NOT for nodes
        std::mutex mut;

        /**
         * @brief NetSelect of every socket
         */
        std::unique_ptr<NetSelect> netsel;

        /**
         * @brief Relay (one or none)
         */
        std::shared_ptr<N2NP::Relay> relay;
        int nsRelayId;

        // Node index (map index => index)
        std::map<int, int> nodeIndexes;
        class OpenConnThread : public Thread
        {
        public:
            OpenConnThread(API *api);
        protected:
            void run();
        private:
            API *api;
        };
    };
}

#endif /* EPYX_API_H */
