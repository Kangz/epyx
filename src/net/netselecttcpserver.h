/**
 * @file netselecttcpserver.h
 * @brief Implementation of NetSelectReader on TCP server
 */

#ifndef EPYX_NETSELECTTCPSERVER_H
#define EPYX_NETSELECTTCPSERVER_H

#include "netselectsocket.h"
#include "tcpserver.h"
#include "tcpsocket.h"

namespace Epyx
{
    /**
     * @class NetSelectTCPServer
     *
     * @brief Manage a TCPServer in NetSelect
     *
     * T is a class which derives from NetSelectSocket and which is intended to
     * be instanced for each incomming connection.
     */
    template<typename T>class NetSelectTCPServer : public NetSelectReader
    {
    public:
        /**
         * NOTE: TCPServer *srv is DELETED by the desctructor
         */
        NetSelectTCPServer(TCPServer *srv);
        ~NetSelectTCPServer();

        int getFileDescriptor();

        bool read();

    protected:
        TCPServer *srv;
    };
}

#include "netselecttcpserver-detail.h"

#endif /* EPYX_NETSELECTTCPSERVER_H */
