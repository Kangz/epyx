/**
 * @file tcpserver.h
 * @brief TCP server implementation
 */

#ifndef EPYX_TCPSERVER_H
#define EPYX_TCPSERVER_H

#include "server.h"

namespace Epyx
{
    /**
     * @class TCPServer
     * @brief TCP Server implementation
     */
    class TCPServer : public Server
    {
    public:
        /**
         * @brief Create a new TCP server
         * @param addr IP address and TCP port to bind to
         * @param nbConn maximum connection number
         */
        TCPServer(const Address& addr, unsigned int nbConn);
    };

}
#endif /* EPYX_TCPSERVER_H */
