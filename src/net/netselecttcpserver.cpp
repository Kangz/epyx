#include "netselecttcpserver.h"
#include "netselect.h"
#include "../core/common.h"

namespace Epyx
{
    NetSelectTCPServer::NetSelectTCPServer(TCPServer &srv)
        :srv(srv)
    {
    }

    bool NetSelectTCPServer::read()
    {
        struct sockaddr_storage clientAddr;
        socklen_t clientAddrLen;
        int newfd = -1;
        NetSelectSocket *nssock = NULL;

        clientAddrLen = sizeof clientAddr;
        newfd = ::accept(srv.getFd(), (struct sockaddr*)&clientAddr,
                        &clientAddrLen);
        if (newfd == -1)
            throw ErrException("NetSelectTCPServer::read", "accept");

        // Encapsulate socket
        try {
            TCPSocket newSock(newfd, Address((struct sockaddr*)&clientAddr));
            nssock = this->selectSocket(newSock);
        } catch (Exception e) {
            log::error << "Unable to setup the link:\n" << e << log::endl;
            if (newfd >= 0) {
                // newfd is managed by newSock... this may bug
                ::close(newfd);
            }
            return true;
        }
        EPYX_ASSERT(nssock != NULL);
        this->getOwner()->add(*nssock);
        return true;
    }
}
