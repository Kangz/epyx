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
        class Listener : public Thread
        {
        public:
            Listener(TCPServer *srv);
            std::unique_ptr<TCPSocket>& getSocket();
            SockAddress getAddress();
            SockAddress getLocalAddress();
            bool hasAccepted();
        protected:
            void run();
        private:
            std::unique_ptr<TCPServer> srv;
            std::unique_ptr<TCPSocket> sock;
            bool hasAccept;
            char* data;
        };

    } // namespace DirectConnection
} // namespace Epyx

#endif // EPYX_DIRECTCONNECTION_LISTENER_H
