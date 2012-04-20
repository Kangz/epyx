/**
 * @file node.h
 * @brief N2NP node implementation
 */

#ifndef EPYX_N2NP_NODE_H
#define EPYX_N2NP_NODE_H

#include "packet.h"
#include "../net/netselectsocket.h"
#include "../parser/gttparser.h"

namespace Epyx
{
    namespace N2NP
    {
        // For ReceiveCb
        class Node;
        /**
         * @class Node
         * @brief N2NP node
         */
        class Node : public NetSelectSocket
        {
        private:
            //Internal definition used for the callback
            typedef bool (ReceiveCb) (Node&, const Packet&, void* cbData);

        public:
            /**
             * @brief Create a new node
             * @param addr Relay Address (IP:port)
             */
            Node(const Address& addr);

            /**
             * @brief Send packet to another node
             * @param to destination Node ID
             * @param method
             * @param size
             * @param body
             * @return true on success
             */
            bool send(const NodeId& to, const std::string& method,
                    unsigned long size, const char *body);

            /**
             * @brief Register a receive callback
             * @param type
             * @param cb
             * @param cbData
             */
            void registerRecv(const PacketType& type, ReceiveCb *cb, void* cbData);

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

            // This ID
            NodeId nodeid;
            bool hasId;
            typedef struct ReceiveCbData
            {
                ReceiveCb* cb;
                void* arg;
            } ReceiveCbData;

            // Callbacks for Recv
            std::map<PacketType, ReceiveCbData> recvCallbacks;
            Mutex recvCallbacksMutex;

            // GTT parser
            GTTParser gttparser;

            // Disable copy
            Node(const Node&);
            const Node& operator=(const Node&);
        };
    }
}

#endif /* EPYX_N2NP_NODE_H */

