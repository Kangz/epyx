#ifndef UDPSOCKET_H
#define UDPSOCKET_H
#include "socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>


namespace Epyx {
    class UDPSocket : public Socket
    {
        public:
            UDPSocket();
        private:
            int ans_sock;
    };
    
}
#endif // UDPSOCKET_H
