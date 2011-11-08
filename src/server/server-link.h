/**
 * Server link, which contains useful information about a connection
 */
#ifndef EPYX_SERVER_LINK_H
#define EPYX_SERVER_LINK_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "../core/address.h"

namespace Epyx
{
    class ServerLink
    {
    private:
        Address clientAddr;
        Address srvAddr;
        int sockfd;
    public:
        ServerLink(struct sockaddr *aiSrv, struct sockaddr *aiClient, int sockfd_);
    };
}

#endif /* EPYX_SERVER_LINK_H */
