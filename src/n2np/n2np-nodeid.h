/**
 * @file n2np-nodeid.h
 * @brief N2NP Node Identifier
 */

#ifndef EPYX_N2NP_NODE_ID_H
#define EPYX_N2NP_NODE_ID_H

#include "../net/address.h"
#include <string>

namespace Epyx
{
    const size_t N2npNodeNameMaxLen = 200;
    /**
     * @class NodeId
     * @brief Manage node ID
     */
    class N2npNodeId
    {
    public:
        /**
         * @brief empty constructor
         */
        N2npNodeId();
        /**
         * @brief Build a relay ID
         * @param relay relay Address
         */
        N2npNodeId(const Address& relay);
        /**
         * @brief Build a node ID (with a name and a relay)
         * @param name
         * @param relay
         */
        N2npNodeId(const std::string& name, const Address& relay);
        /**
         * @brief Build a node ID from "node@relay" expression
         * @param str string "node@relay"
         */
        N2npNodeId(const std::string& str);
        /**
         * @brief Copy constructor
         */
        N2npNodeId(const N2npNodeId& nodeid);
        /**
         * @brief Assignment
         */
        N2npNodeId& operator=(const N2npNodeId& nodeid);

        /**
         * @brief Print the node ID in an output stream
         * @param os output stream
         * @param addr
         */
        friend std::ostream& operator<<(std::ostream& os, const N2npNodeId& nodeid);

        /**
         * @brief Return a std::string representation of the address
         * @return std::string
         */
        std::string toString() const;

        /**
         * @brief Equality test
         * @param nodeid1
         * @param nodeid2
         */
        friend bool operator==(const N2npNodeId& nodeid1, const N2npNodeId& nodeid2);

        /**
         * @brief Inequality test
         * @param nodeid1
         * @param nodeid2
         */
        friend bool operator!=(const N2npNodeId& nodeid1, const N2npNodeId& nodeid2);

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

    private:
        std::string name;
        Address relay;
    };
}

#endif /* EPYX_N2NP_NODE_ID_H */
