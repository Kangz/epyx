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
#include <condition_variable>
#include <string>
#include <map>
#include "../n2np/nodeid.h"
#include "../n2np/node.h"

namespace Epyx
{
    namespace DirectConnection
    {
        typedef enum State
        {
            STATE_CLIENT,
            STATE_SERVER
        } State;
        typedef enum Method
        {
            DIRECT,
            UPNP
        } Method;
        /**
         * @class OpenConnection
         * @brief Manage a state machine for openning a connection
         * 
         * Try following methods:
         *  - Direct : Client directly connects to the server.
         *       This works since one node is not behind a NAT nor a firewall
         *  - UPnP : Server configures it's IGD to redirect connections
         *       This works when both nodes are behind NAT and a NAT does UPnP-IGD
         */
        class OpenConnection
        {
        public:
            /**
             * @brief Initialise the state Machine
             * @param node
             * @param remoteHost
             * @param clients
             */
            OpenConnection(const std::shared_ptr<N2NP::Node>& node,
                    const N2NP::NodeId& remoteHost, bool clients = true);

            ~OpenConnection();

            /**
             * @brief Advance from a state to another by processing a message from N2NP system
             * @param command
             * @param headers
             */
            void getMessage(const std::string& command, const std::map<std::string, std::string>& headers);

            /**
             * @brief Name of method in N2NP packets
             */
            static const std::string n2npMethodName;

            /**
             * @brief Name of protocol in GTT packets
             */
            static const std::string protoName;

        private:
            /**
             * @brief Runned function of the thread
             */
            void run();

            /**
             * @brief Start next method if the current one failed
             * @return true if hope exists, false if every methods were tried
             */
            bool tryNextMethod();

            /**
             * @brief Do TCP server logic
             * @return true if a socket has been obtained
             */
            bool serverStateOpen();

            N2NP::NodeId remoteHost;
            State state;
            bool client_tried;
            bool server_tried;
            Method tested_method;
            std::unique_ptr<TCPSocket> socket;
            std::shared_ptr<N2NP::Node> node;

            bool receivedDidNotWork;
            bool isEstablished;
            std::condition_variable condition;

            std::thread running_thread;
        };
    }
}

#endif // EPYX_DIRECTCONNECTION_OPENCONNECTION_H
