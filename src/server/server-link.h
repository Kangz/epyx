/**
 * Server link, which contains useful information about a connection
 */
#ifndef EPYX_SERVER_LINK_H
#define EPYX_SERVER_LINK_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ostream>
#include "../core/address.h"
#include "server-listener.h" /* for ServerRun */

namespace Epyx
{
    class ServerLink
    {
    private:
        int sockfd;
    public:
        ServerListener *listener;
        Address clientAddr;
        Address *srvAddr;

        ServerLink(ServerListener *listener, Address *srvAddr_,
                   struct sockaddr *aiClient, int sockfd_);
        ~ServerLink();

        void close();

        /**
         * send may not send everything if the system tells so
         */
        unsigned int send(const void *data, int size);
        bool sendAll(const void *data, int size);
        bool sendText(const char *text);

        /**
         * receive data, blocking calls
         */
        unsigned int recv(void *data, int size);
        bool recvAll(void *data, int size);
        bool recvLine(std::ostream& out);
    };
}

#endif /* EPYX_SERVER_LINK_H */
