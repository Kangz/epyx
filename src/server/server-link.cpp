#include "server-link.h"

namespace Epyx
{
    ServerLink::ServerLink(struct sockaddr *aiSrv, struct sockaddr *aiClient, int sockfd_)
        :srvAddr(aiSrv), clientAddr(aiClient), sockfd(sockfd_)
    {
        std::cout << ">> Incoming from " << clientAddr << " for " << srvAddr << "\n";
    }
}
