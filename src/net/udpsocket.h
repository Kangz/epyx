#ifndef EPYX_UDPSOCKET_H
#define EPYX_UDPSOCKET_H

#include "socket.h"

namespace Epyx {
    class UDPServer;

    class UDPSocket : public Socket
    {
    public:
        UDPSocket(const Address &addr);
        UDPSocket();
        UDPSocket(int sockfd, const Address &addr);
        int recv(void*, int);
        int send(const void *data, int size);
        Address getLastRecvAddr();
        
    private:
        Address lastRecvAddr;
    };

}
#endif // EPYX_UDPSOCKET_H
