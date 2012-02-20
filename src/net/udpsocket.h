#ifndef EPYX_UDPSOCKET_H
#define EPYX_UDPSOCKET_H

#include "socket.h"
#include "udpserver.h"

namespace Epyx {
    class UDPSocket : public Socket
    {
    public:
        UDPSocket(bool reply = false);
        void close();
        int recv(void*, int);
        bool connect();

    private:
        // Use an UDP server to get answers, if they are waited
        UDPServer *answerServ;
    };

}
#endif // EPYX_UDPSOCKET_H
