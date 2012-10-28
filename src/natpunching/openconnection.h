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
#ifndef EPYX_DIRECTCONNECTION_OPENCONNECTION_H
#define EPYX_DIRECTCONNECTION_OPENCONNECTION_H
#include "../core/thread.h"
#include "../net/tcpsocket.h"
#include <string>
#include <map>
#include "../n2np/nodeid.h"
#include "../n2np/node.h"

namespace Epyx
{
    namespace DirectConnection
    {
        typedef enum state
        {
            STATE_CLIENT,
            STATE_SERVER
        } state;
        typedef enum method
        {
            DIRECT,
            UPNP
        } method;
        class OpenConnection : public Thread
        {
        public:
            /**
             * @brief Initialise the state Machine
             */
            OpenConnection(const std::shared_ptr<N2NP::Node>& node,
                    const N2NP::NodeId& remoteHost, bool clients = true);

            /**
             * @brief Advance from a state to another
             * @param command
             * @param headers
             */
            void getMessage(const std::string& command, const std::map<std::string, std::string>& headers);
        protected:
            void run();
        private:
            void serverStateOpen();
            N2NP::NodeId remoteHost;
            state etat;
            bool client_tried;
            bool server_tried;
            method tested_method;
            std::unique_ptr<TCPSocket> socket;
            std::shared_ptr<N2NP::Node> node;
        };
    } // namespace DirectConnection
} // namespace Epyx

#endif // EPYX_DIRECTCONNECTION_OPENCONNECTION_H
