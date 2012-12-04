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
#ifndef EPYX_DIRECTCONNECTION_LISTENER_H
#define EPYX_DIRECTCONNECTION_LISTENER_H
#include "../net/netselectsocket.h"
#include "../net/uri.h"
#include "../net/tcpsocket.h"
#include "../net/tcpserver.h"
#include "../n2np/node.h"

namespace Epyx
{
    namespace DirectConnection
    {
        /**
         * @class Listener
         * 
         * @brief Thread to listen to a TCP connection
         */
        class Listener : public Thread
        {
        public:
            /**
             * @brief Build a new listener with a new TCPServer
             * @param srv
             */
            Listener(TCPServer *srv);

            /**
             * @brief retrieve internal socket and RESET it
             * @return ptr to a TCP socket
             */
            std::unique_ptr<TCPSocket> retrieveSocket();

            /**
             * @brief Return true if the socket has been accepted
             * @return bool
             */
            bool hasAccepted();

            /**
             * @brief Get server address
             * @return SockAddress
             */
            SockAddress getListenAddress() const;

        protected:
            void run();

        private:
            std::unique_ptr<TCPServer> srv;
            std::unique_ptr<TCPSocket> sock;
            bool hasAccept;
        };

    } // namespace DirectConnection
} // namespace Epyx

#endif // EPYX_DIRECTCONNECTION_LISTENER_H
