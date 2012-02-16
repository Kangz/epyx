#ifndef EPYX_TCPSOCKET_H
#define EPYX_TCPSOCKET_H

#include "socket.h"

namespace Epyx
{
    class TCPSocket : public Socket
    {
        public:
            TCPSocket();
            TCPSocket(Address&);
    };

}
#endif // EPYX_UDPSOCKET_H
