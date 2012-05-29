/**
 * @file api.h
 * @brief Application Programming Interface of Epyx
 */

#ifndef EPYX_API_H
#define EPYX_API_H

#include "core/common.h"
#include "core/atom/counter.h"
#include "core/atom/map.h"
#include "net/netselect.h"
#include "n2np/relay.h"
#include "n2np/node.h"
#include <list>

namespace Epyx
{
    class API
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
        void spawnRelay(const Address& addr, unsigned int nbConn = 50);

        /**
         * @brief Destroy specified relay
         * @param addr
         * @param detachTime timeout to wait for detach nodes
         */
        void destroyRelay(const Timeout& detachTime);

        /**
         * @brief Add a new node
         * @param node a dynamicaly-allocated node
         *
         * node is deleted by API, NOT by the caller
         * 
         * @return identifier
         */
        int addNode(N2NP::Node *node);

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
        // Disable copy construction and assignment.
        API(const API&);
        const API& operator=(const API&);

        // Main mutex, for netsel and relay, NOT for nodes
        Mutex mut;

        /**
         * @brief NetSelect of every socket
         */
        NetSelect *netsel;

        /**
         * @brief Relay (one or none)
         */
        N2NP::Relay *relay;
        int nsRelayId;

        // Node index (map index => index)
        atom::Map<int, int> nodeIndexes;
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
