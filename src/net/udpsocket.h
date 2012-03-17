#ifndef EPYX_UDPSOCKET_H
#define EPYX_UDPSOCKET_H

#include "socket.h"

namespace Epyx {
    class UDPServer;

    class UDPSocket : public Socket
    {
    public:
        UDPSocket(bool reply = false);
        void close();
        int recv(void*, int);
        bool connect();

        int getRecvFd();

    private:
        // Use an UDP server to get answers, if they are waited
        UDPServer *answerServ;
    };

}
#endif // EPYX_UDPSOCKET_H
