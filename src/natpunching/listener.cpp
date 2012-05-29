#include "listener.h"

namespace Epyx {
    namespace DirectConnection {
        
        Listener::Listener(TCPServer *srv)
            :srv(srv), hasAccept(false)
        {
        }
        void Listener::run(){
            struct sockaddr_storage clientAddr;
            socklen_t clientAddrLen;
            struct sockaddr_storage localAddr;
            socklen_t localAddrLen;
            
            clientAddrLen = sizeof clientAddr;
            localAddrLen = sizeof localAddr;
            
            int newfd = srv->getFd();
            
            ::accept(newfd,(struct sockaddr*) &clientAddr, &clientAddrLen);
            hasAccept = true;
            sock = new TCPSocket(newfd, Address((struct sockaddr*) &clientAddr));
            ::getsockname(newfd,(struct sockaddr*) &localAddr,&localAddrLen);
            sock->setLocalAddress(Address((struct sockaddr*) &localAddr));
        }
        Address Listener::getAddress(){
            if (sock != NULL)
                return sock->getAddress();
            else
                return Address();
        }
        bool Listener::hasAccepted(){
            return this->hasAccept;
        }

        Address Listener::getLocalAddress(){
            struct sockaddr_storage localAddr;
            socklen_t localAddrLen;
            localAddrLen = sizeof localAddr;
            
            ::getsockname(sock->getFd(),(struct sockaddr*) &localAddr,&localAddrLen);
            return Address ( (struct sockaddr*) &localAddr);
        }
        TCPSocket * Listener::getSocket(){
            return sock;
        }
    } // namespace DirectConnection
} // namespace Epyx
