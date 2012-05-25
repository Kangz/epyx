/**
 * @file udpserver.h
 * @brief UDP server abstraction layer
 */
#ifndef EPYX_UDPSERVER_H
#define EPYX_UDPSERVER_H

#include "server.h"
#include "udpsocket.h"

namespace Epyx
{
    /**
     * @class UDPServer
     * @brief UDP server abstraction layer
     */
    class UDPServer : public Server
    {
    public:
        /**
         * @brief Create a new UDP server
         * @param port UDP port to bind to
         */
        UDPServer(const Address& addr);

        /**
         * @brief Receive data for the server
         *
         * @param data received data buffer
         * @param size size of data buffer
         * @return number of received bytes
         */
        int recv(void *data, int size);

        // send is not implemented yet
        //int send(const void *data, int size);

    private:
        UDPSocket sock;
    };
}
#endif /* EPYX_UDPSERVER_H */

