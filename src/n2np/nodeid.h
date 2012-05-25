/**
 * @file nodeid.h
 * @brief N2NP Node Identifier
 */

#ifndef EPYX_N2NP_NODE_ID_H
#define EPYX_N2NP_NODE_ID_H

#include "../net/address.h"
#include <string>

namespace Epyx
{
    namespace N2NP
    {
        /**
         * @brief Maximum length of a node name
         */
        const size_t NodeNameMaxLen = 200;
        /**
         * @class NodeId
         * @brief Manage node ID
         */
        class NodeId
        {
        public:
            /**
             * @brief empty constructor
             */
            NodeId();
            /**
             * @brief Build a relay ID
             * @param relay relay Address
             */
            NodeId(const Address& relay);
            /**
             * @brief Build a node ID (with a name and a relay)
             * @param name
             * @param relay
             */
            NodeId(const std::string& name, const Address& relay);
            /**
             * @brief Build a node ID from "node@relay" expression
             * @param str string "node@relay"
             */
            NodeId(const std::string& str);
            /**
             * @brief Copy constructor
             */
            NodeId(const NodeId& nodeid);
            /**
             * @brief Assignment
             */
            NodeId& operator=(const NodeId& nodeid);

            /**
             * @brief Print the node ID in an output stream
             * @param os output stream
             * @param nodeid
             */
            friend std::ostream& operator<<(std::ostream& os, const NodeId& nodeid);

            /**
             * @brief Return a std::string representation of the address
             * @return std::string
             */
            std::string toString() const;

            /**
             * @brief compare this and nodeid
             * @param addr
             */
            int compare(const NodeId& nodeid) const;

            /**
             * @brief Equality test
             * @param nodeid1
             * @param nodeid2
             */
            friend bool operator==(const NodeId& nodeid1, const NodeId& nodeid2);

            /**
             * @brief Inequality test
             * @param nodeid1
             * @param nodeid2
             */
            friend bool operator!=(const NodeId& nodeid1, const NodeId& nodeid2);

            /**
             * Less test
             * @param nodeid1
             * @param nodeid2
             */
            friend bool operator<(const NodeId& nodeid1, const NodeId& nodeid2);

            /**
             * @brief Get Node name
             * @return this->name
             */
            const std::string& getName() const;

            /**
             * @brief Get relay address
             * @return this->relay
             */
            const Address& getRelay() const;

            /**
             * @brief is empty
             * @return true if it is empty
             */
            bool empty() const;

        private:
            std::string name;
            Address relay;
        };
    }
}

#endif /* EPYX_N2NP_NODE_ID_H */
