#include "netselect.h"
#include "../core/common.h"
#include "server.h"

namespace Epyx
{

    template<class T, typename TP>
    NetSelectTCPServer<T, TP>::NetSelectTCPServer(TCPServer *srv, TP *param)
    : srv(srv), param(param) {
        EPYX_ASSERT(srv != NULL);
        // Force bind
        if (!srv->isBinded())
            throw FailException("NetSelectTCPServer", "Unbinded server");
    }

    template<class T, typename TP> NetSelectTCPServer<T, TP>::~NetSelectTCPServer() {
        if (srv != NULL) {
            delete srv;
            srv = NULL;
        }
    }

    template<class T, typename TP> int NetSelectTCPServer<T, TP>::getFileDescriptor() const {
        EPYX_ASSERT(srv != NULL);
        return srv->getFd();
    }

    template<class T, typename TP> bool NetSelectTCPServer<T, TP>::read() {
        EPYX_ASSERT(srv != NULL);
        struct sockaddr_storage clientAddr;
        socklen_t clientAddrLen;
        int newfd = -1;
        TCPSocket *newSock = NULL;

        clientAddrLen = sizeof clientAddr;
        int srvfd = srv->getFd();
        if (srvfd < 0)
            return false;
        newfd = ::accept(srvfd, (struct sockaddr*) &clientAddr,
                &clientAddrLen);
        if (newfd == -1) {
            if (!srv->isBinded())
                return false;
            throw ErrException("NetSelectTCPServer::read", "accept");
        }

        // Encapsulate socket
        try {
            newSock = new TCPSocket(newfd, Address((struct sockaddr*) &clientAddr));
            newSock->setLocalAddress(srv->getAddress());
            T *nsSocket = new T(newSock, param);
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
