/**
 * @file netselectsocket.h
 * @brief Implementation of NetSelectReader on sockets
 */

#ifndef EPYX_NETSELECTSOCKET_H
#define EPYX_NETSELECTSOCKET_H

#include "netselectreader.h"
#include "socket.h"

namespace Epyx
{
    class NetSelectSocket : public NetSelectReader
    {
    public:
        NetSelectSocket(Socket &sock);

        /**
         * @brief Create a new NetSelectSocket with a dynamicaly allocated socket
         */
        NetSelectSocket(Socket *psock);

        ~NetSelectSocket();

        int getFileDescriptor();

        bool read();

        /**
         * Eat read data
         */
        virtual void eat(const char *data, long size) = 0;

    protected:
        /**
         * @brief Get internal socket
         */
        Socket& socket();

    private:
        // sock may be deleted by this object to prevent memory leak
        Socket *sock;
        bool dynamicSock;
    };
}
#endif /* EPYX_NETSELECTREADER_H */
