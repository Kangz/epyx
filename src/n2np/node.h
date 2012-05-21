/**
 * @file node.h
 * @brief N2NP node implementation
 */

#ifndef EPYX_N2NP_NODE_H
#define EPYX_N2NP_NODE_H

#include "packet.h"
#include "module.h"
#include "../net/netselectsocket.h"
#include "../parser/gttparser.h"
#include "../core/atom/counter.h"
#include "../core/atom/map.h"

namespace Epyx
{
    namespace N2NP
    {
        class Module; //It does exist, it tell you.

        class Node;

        /**
         * @class Node
         * @brief N2NP node
         */
        class Node : public NetSelectSocket
        {
        public:
            /**
             * @brief Create a new node
             * @param addr Relay Address (IP:port)
             */
            Node(const Address& addr);

            /**
             * @brief Send an N2NP packet to another node
             * @param to destination Node ID
             * @param method
             * @param data
             * @param size
             * @return true on success
             */
            bool send(const NodeId& to, const std::string& method,
                    const char *data, unsigned long size, bool store = true);

            /**
             * @brief Send an ancapsulated GTT packet packet to another node
             *
             * First build the GTT packet, then add N2NP headers, and send the
             * whole through network
             *
             * @param to destination Node ID
             * @param method
             * @param pkt holy packet
             * @return true on success
             */
            bool send(const NodeId& to, const std::string& method,
                    const GTTPacket& pkt);

            /**
             * @brief Register a new Module
             * @param method Which method to use
             * @param m The module to call
             */
            void addModule(std::string method, Module *m);

            /**
             * @brief Get node ID
             * @return this->nodeid
             */
            const NodeId& getId() const;

            /**
             * @brief Test if node is ready
             * @return true if it is
             */
            bool isReady() const;

        protected:
            /**
             * @brief Eat data and treat each N2NP packet it contains
             */
            void eat(const char *data, long size);

        private:
            /**
             * @brief Treat a N2NP packet
             */
            void treat(Packet *pkt);
            void sendAck(Packet *pkt);
            void sendErr(Packet *pkt);

            // This ID
            NodeId nodeid;
            bool hasId;

            // Callbacks for Recv
            std::map<std::string, Module*> modules;
            Mutex modulesMutex;

            // GTT parser
            GTTParser gttparser;

            // Most recently used node IDs.
            // Each node has a number which corresponds to a weight.
            atom::Map<NodeId, unsigned int> mruNodeIds;

            // Disable copy
            Node(const Node&);
            const Node& operator=(const Node&);

            atom::Counter curId;
            std::map<unsigned long, Packet*> sentMap;
        };
    }
}

#endif /* EPYX_N2NP_NODE_H */

