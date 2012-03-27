#include "netselect.h"
#include "../core/common.h"

namespace Epyx
{

    template<typename T> NetSelectTCPServer<T>::NetSelectTCPServer(TCPServer *srv)
    : srv(srv) {
        EPYX_ASSERT(srv != NULL);
        // Force bind
        if (!srv->isBinded())
            throw FailException("NetSelectTCPServer", "Unbinded server");
    }

    template<typename T> NetSelectTCPServer<T>::~NetSelectTCPServer() {
        if (srv != NULL) {
            delete srv;
            srv = NULL;
        }
    }

    template<typename T> int NetSelectTCPServer<T>::getFileDescriptor() const {
        EPYX_ASSERT(srv != NULL);
        return srv->getFd();
    }

    template<typename T> bool NetSelectTCPServer<T>::read() {
        EPYX_ASSERT(srv != NULL);
        struct sockaddr_storage clientAddr;
        socklen_t clientAddrLen;
        int newfd = -1;
        TCPSocket *newSock = NULL;

        clientAddrLen = sizeof clientAddr;
        newfd = ::accept(srv->getFd(), (struct sockaddr*) &clientAddr,
                &clientAddrLen);
        if (newfd == -1)
            throw ErrException("NetSelectTCPServer::read", "accept");

        // Encapsulate socket
        try {
            newSock = new TCPSocket(newfd, Address((struct sockaddr*) &clientAddr));
            newSock->setLocalAddress(srv->getAddress());
            T *nsSocket = new T(newSock);
            this->getOwner()->add(nsSocket);
        } catch (Exception e) {
            log::error << "Unable to setup the link:\n" << e << log::endl;
            if (newSock != NULL) {
                // newfd is managed by newSock
                delete newSock;
                newfd = -1;
            } else if (newfd >= 0) {
                ::close(newfd);
            }
            return true;
        }
        return true;
    }
}
