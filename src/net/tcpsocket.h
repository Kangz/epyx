#ifndef EPYX_TCPSOCKET_H
#define EPYX_TCPSOCKET_H

#include "socket.h"

namespace Epyx
{
    class TCPSocket : public Socket
    {
    public:
        TCPSocket();
        TCPSocket(const Address&);
        TCPSocket(int sock, const Address &addr);
        
        int send(const void *data, int size);
        int recv(void *data, int size);
    private:
        bool connect();
        bool isConnected;
    };

}
#endif // EPYX_UDPSOCKET_H
