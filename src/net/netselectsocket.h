/**
 * @file netselecttcpsocket.h
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

        inline int getFileDescriptor()
        {
            return sock.getRecvFd();
        }

        bool read();

        /**
         * Eat read data
         */
        virtual void eat(const char *data, long size) = 0;

    private:
        Socket &sock;
    };
}
#endif /* EPYX_NETSELECTREADER_H */
