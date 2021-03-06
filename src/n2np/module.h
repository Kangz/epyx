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
        class Node;

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
             */
            virtual void fromN2NP(Node& node, NodeId from, const byte_str& data) = 0;
        };
    }
}

#endif /* EPYX_N2NP_MODULE_H */
