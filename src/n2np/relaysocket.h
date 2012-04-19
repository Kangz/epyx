/**
 * @file relaysocket.h
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
            RelaySocket(Socket *sock, Relay *relay);
            ~RelaySocket();

        protected:
            void eat(const char *data, long size);

        private:
            NodeId nodeid;
            Relay *relay;

            GTTParser gttparser;
        };

        /**
         * @brief NetSelectTCPServer for relay
         */
        typedef NetSelectTCPServer<RelaySocket, Relay> RelayServer;
    }
}

#endif /* RELAYSOCKET_H */

