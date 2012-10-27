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
 * @file n2np/relaysocket.h
 * @brief Implement how a Relay is acceded with a Socket
 */

#ifndef EPYX_N2NP_RELAYSOCKET_H
#define EPYX_N2NP_RELAYSOCKET_H

#include "../net/netselectsocket.h"
#include "../net/netselecttcpserver.h"
#include "../parser/gttparser.h"
#include "nodeid.h"

namespace Epyx
{
    namespace N2NP
    {
        class Relay;
        /**
         * @class RelaySocket
         * @brief NetSelectSocket for Relay
         */
        class RelaySocket : public NetSelectSocket
        {
        public:
            /**
             * @brief Constructor for NetSelectTCPServer
             * @param sock new socket
             * @param relay associated relay
             */
            RelaySocket(Socket *sock, const std::shared_ptr<Relay>& relay);
            ~RelaySocket();

        protected:
            void eat(const byte_str& data);

        private:
            NodeId nodeid;
            std::shared_ptr<Relay> relay;

            GTTParser gttparser;
        };

        /**
         * @brief NetSelectTCPServer for relay
         */
        typedef NetSelectTCPServer<RelaySocket, std::shared_ptr<Relay> > RelayServer;
    }
}

#endif /* RELAYSOCKET_H */

