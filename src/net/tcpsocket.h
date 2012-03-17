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

        int getRecvFd();
    };

}
#endif // EPYX_UDPSOCKET_H
