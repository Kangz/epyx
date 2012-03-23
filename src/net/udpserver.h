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
        //UDPServer(unsigned short port,int sockfd);

        bool bind();

        int recv(void *data, int size);
        int send(const void *data, int size);


    private:
        UDPSocket sock;
    };
}
#endif /* EPYX_UDPSERVER_H */

