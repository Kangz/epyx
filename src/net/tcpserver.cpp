#include "tcpserver.h"
#include "../core/assert.h"
#include "../core/log.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

namespace Epyx
{
    TCPServer::TCPServer(unsigned short port, unsigned int nbConn, ServerRun& srvRun)
        :Server(port, nbConn, srvRun)
    {
    }

    TCPServer::~TCPServer()
    {
        // Close socket
       this->close();
    }

    bool TCPServer::bind()
    {
        //EPYX_ASSERT(port < 65536);
        char charport[10];
        struct addrinfo hints, *addrAll, *pai;
        int status, flag;

        // Convert port to char* to find address hints
        snprintf(charport, sizeof(charport), "%u", port);
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;     // AF_INET or AF_INET6 to force IP version
        hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
        hints.ai_flags = AI_PASSIVE;     // Server
        status = ::getaddrinfo(NULL, charport, &hints, &addrAll);
        if (status != 0) {
            FailException e("TCPServer::bind", "getaddrinfo error: ");
            e.append(gai_strerror(status));
            throw e;
        }

        // getaddrinfo() returns a list of address structures.
        // Try each address until we successfully bind.
        for (pai = addrAll; pai != NULL; pai = pai->ai_next) {
            // Create a new socket
            this->sockfd = ::socket(pai->ai_family, pai->ai_socktype, pai->ai_protocol);
            if (this->sockfd == -1)
                continue;

            // Allow reusing the address (ignore failure)
            flag = 1;
            setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));

            // Bind to a listening address/port
            status = ::bind(this->sockfd, pai->ai_addr, pai->ai_addrlen);
            if (status == 0) {
                // Success : remember used address
                this->address = Address(pai->ai_addr);
                break;
            }

            // TODO: Epyx::log::warn << bind error (to see already-in-use addresses)
            log::warn << "Bind status " << status << log::endl;

            // Close socket if bind fails
            ::close(this->sockfd);
            this->sockfd = -1;
        }
        freeaddrinfo(addrAll);
        if (pai == NULL) {
            log::debug << "Nowhere to bind." << log::endl;
            return false;
        }

        // Now there is a socket, listen to nbConn connections
        EPYX_ASSERT(sockfd >= 0);

        // Listening to nbConn connections
        status = listen(sockfd, nbConn);
        if (status  < 0) {
            this->close();
            throw ErrException("TCPSocket", "listen");
        }
        return true;
    }
}
