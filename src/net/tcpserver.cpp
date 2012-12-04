#include "tcpserver.h"
#include "../core/common.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

namespace Epyx
{

    TCPServer::TCPServer(const SockAddress& addr, unsigned int nbConn)
    :Server(addr) {
        if (!this->_internal_bind(SOCK_STREAM))
            return;

        // Now there is a socket, listen to nbConn connections
        EPYX_ASSERT(sockfd >= 0);
        int status = listen(sockfd, nbConn);
        if (status < 0) {
            log::error << "listen: " << log::errstd << log::endl;
            this->close();
            return;
        }
    }

    std::unique_ptr<TCPSocket> TCPServer::accept() {
        struct sockaddr_storage clientAddr;
        socklen_t clientAddrLen = sizeof clientAddr;
        std::unique_ptr<TCPSocket> newSock;

        if (!this->isBinded()) {
            return newSock;
        }
        int newfd = ::accept(sockfd, (struct sockaddr*) &clientAddr, &clientAddrLen);
        if (newfd == -1) {
            // Don't show an error if the server is no longer binded
            if (!this->isBinded()) {
                return newSock;
            }
            throw ErrException("NetSelectTCPServer::read", "accept");
        }

        // Encapsulate socket
        newSock.reset(new TCPSocket(newfd, SockAddress((struct sockaddr*) &clientAddr)));

        // In usual cases, newSock->updateLocalAddress fill the local address.
        // If it's not the case, use server binding address
        if (newSock->getLocalAddress().empty()) {
            newSock->setLocalAddress(this->getAddress());
        }
        return newSock;
    }
}
