/**
 * @file netselecttcpsocket.h
 * @brief Implementation of NetSelectReader on sockets
 */

#ifndef EPYX_NETSELECTTCPSERVER_H
#define EPYX_NETSELECTTCPSERVER_H

#include "netselectsocket.h"
#include "tcpserver.h"
#include "tcpsocket.h"

namespace Epyx
{
    class NetSelectTCPServer : public NetSelectReader
    {
    public:
        NetSelectTCPServer(TCPServer &srv);

        inline int getFileDescriptor()
        {
            return srv.getFd();
        }

        bool read();

        /**
         * @brief Create a new NetSelectSocket from an incomming connection
         */
        virtual NetSelectSocket* selectSocket(TCPSocket &sock) = 0;

    private:
        TCPServer &srv;
    };
}
#endif /* EPYX_NETSELECTTCPSERVER_H */
