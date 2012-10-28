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
#ifndef EPYX_DIRECTCONNECTION_DISPATCHER_H
#define EPYX_DIRECTCONNECTION_DISPATCHER_H

#include "../n2np/module.h"
#include "openconnection.h"

namespace Epyx
{
    namespace DirectConnection
    {
        /**
         * @brief N2NP Module used to manage a Direct Connection
         */
        class Dispatcher : public N2NP::Module
        {
        public:
            /**
             * @brief Add DIRECTCONNECTION module to a N2NP node
             * @param node shared pointer to an N2NP node
             */
            static void addModule(const std::shared_ptr<N2NP::Node>& node);

            /**
             * @brief Receive data from N2NP
             * @param node
             * @param from
             * @param data
             */
            void fromN2NP(N2NP::Node& node, N2NP::NodeId from, const byte_str& data);

        private:
            /**
             * @brief Private constructor, use create instead
             * @param node N2NP shared pointer to be associated with
             */
            Dispatcher(const std::shared_ptr<N2NP::Node>& node);

            std::shared_ptr<N2NP::Node> node;

            // Map of state machines in node connection
            std::mutex nodeConnectMutex;
            std::map<N2NP::NodeId, std::shared_ptr<OpenConnection> > nodeConnect;
        };
    } // namespace DirectConnection
} // namespace Epyx

#endif // EPYX_DIRECTCONNECTION_DISPATCHER_H
