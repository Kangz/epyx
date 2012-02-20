#include "udpserver.h"

namespace Epyx
{
    UDPServer::UDPServer(unsigned short port)
        :Server(port)
    {
    }

    UDPServer::~UDPServer()
    {
    }

    bool UDPServer::bind()
    {
        if (!this->_internal_bind(SOCK_DGRAM))
            return false;

        // Directly create a socket to receive connections
        sock.setAddress(this->address);
        sock.setFd(sockfd);
        return true;
    }
}
