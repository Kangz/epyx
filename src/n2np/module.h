/**
 * @file n2np/module.h
 * @brief N2NP module
 */
#ifndef EPYX_N2NP_MODULE_H
#define EPYX_N2NP_MODULE_H

#include "node.h"
#include "nodeid.h"

namespace Epyx
{
    namespace N2NP
    {
        class Node; //Yes, it exists.
        class Module;
        /**
         * @class Module
         * @brief A virtual class for N2NP Modules
         */
        class Module
        {
            public:
                /**
                 * @brief Gets relevant info from N2NP
                 * @param node the node which received the message (useful for replying)
                 * @param from the nodeId of the sender
                 * @param data your data
                 * @param size the data size
                 */
                virtual void fromN2NP(Node& node, NodeId from, const char* data, unsigned int size)=0;
        };
    }
}

#endif /* EPYX_N2NP_MODULE_H */
