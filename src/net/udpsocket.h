#ifndef EPYX_UDPSOCKET_H
#define EPYX_UDPSOCKET_H

#include "socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <cstring>

namespace Epyx {
    class UDPSocket : public Socket
    {
        public:
            UDPSocket(bool reply = false);
            void close();
            int recv(void*, int);
            void connect();
        private:
            bool reply;
            int ans_sock;
    };

}
#endif // EPYX_UDPSOCKET_H
