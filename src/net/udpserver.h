#ifndef EPYX_UDPSERVER_H
#define EPYX_UDPSERVER_H

#include "server.h"
#include "udpsocket.h"

namespace Epyx
{
    class UDPServer : public Server
    {
    public:
        UDPServer(unsigned short port);
        ~UDPServer();

        bool bind();

        inline int recv(void *data, int size)
        {
            return sock.recv(data, size);
        }

    private:
        UDPSocket sock;
    };
}
#endif /* EPYX_UDPSERVER_H */
