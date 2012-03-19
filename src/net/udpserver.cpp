#include "udpserver.h"
#include "../core/common.h"


namespace Epyx
{
    UDPServer::UDPServer(unsigned short port)
        :Server(port)
    {
    }
/*    UDPServer::UDPServer(unsigned short port, int sockfd)
        :Server(port), sockfd(sockfd)
    {        
    }
*/

    bool UDPServer::bind()
    {
        if (!this->_internal_bind(SOCK_DGRAM))
            return false;

        // Directly create a socket to receive connections
        sock.setFd(sockfd);
        sock.setLocalAddress(address);
        return true;
    }
    int UDPServer::recv(void *data, int size){
        int r = sock.recv(data, size);
        //sock.setAddress(sock.getLastRecvAddr());// This is not thread-safe
        return r;
    }
    int UDPServer::send(const void *data, int size){
        throw FailException("UDPServer", "UDPServer::send is not yet implemented");
        return sock.send(data, size);
    }
}
