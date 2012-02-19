#include "server.h"
#include "../core/assert.h"
#include "../core/log.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

namespace Epyx
{
    Server::Server(unsigned short port, unsigned int nbConn, ServerRun& srvRun)
        :srvRun(srvRun), port(port), nbConn(nbConn), sockfd(-1)
    {
    }

    Server::~Server()
    {
        // Close socket
       this->close();
    }

    const Address& Server::getAddress()
    {
        return address;
    }

    void Server::close()
    {
        if (sockfd >= 0) {
            ::shutdown(sockfd, SHUT_RDWR);
            ::close(sockfd);
            sockfd = -1;
        }
        running = false;
    }

    void Server::run()
    {
        int newfd = -1;
        struct sockaddr_storage clientAddr;
        socklen_t clientAddrLen;
        Socket *newSock = NULL;
        ServerRunnable *srun = NULL;
        int spawnId = 1;

        // Bind if no socket available
        if (this->sockfd < 0 && !this->bind())
            throw FailException("Server::run", "Unable to bind");

        EPYX_ASSERT(this->sockfd >= 0);
        log::debug << "Listening on " << this->address << log::endl;
        try {
            this->running = true;
            while (this->running && this->sockfd >= 0) {
                clientAddrLen = sizeof clientAddr;
                newfd = ::accept(this->sockfd, (struct sockaddr*)&clientAddr,
                               &clientAddrLen);

                // If this thread was not killed, return
                if (!this->running || this->sockfd < 0) {
                    ::shutdown(newfd, SHUT_RDWR);
                    ::close(newfd);
                    newfd = -1;
                    break;
                }
                if (newfd == -1)
                    throw ErrException("Server::run", "accept");

                // Encapsulate socket
                try {
                    newSock = new Socket(newfd, Address((struct sockaddr*)&clientAddr));
                } catch (Exception e) {
                    log::error << "Unable to setup the link:\n" << e << log::endl;
                    if (newSock)
                        delete newSock;
                    else if (newfd >= 0) {
                        // newfd is managed by newSock
                        ::close(newfd);
                    }
                    newSock = NULL;
                }
                // Make newfd variable invalid
                newfd = -1;

                if (newSock != NULL) {
                    try {
                        srun = new ServerRunnable(this->srvRun, this, newSock);
                        std::stringstream portSStream;
                        Thread t(srun, "client", spawnId++);//TODO: Set server-internal name
                        t.run();
                    } catch (Exception e) {
                        log::error << e << log::endl;
                        log::error << "Unable to start a thread" << log::endl;
                        if (srun != NULL)
                            delete srun;
                        delete newSock;
                    }
                    // DO NOT delete newSock nor srun as they are owned by an other thread
                    newSock = NULL;
                    srun = NULL;
                }
            }
        } catch (Exception e) {
            log::error << e << log::endl;
            log::error << "Arg ! An exception killed me !" << log::endl;
            if (newSock)
                delete newSock;
        }
        log::debug << "Clean exit on " << this->address << log::endl;
    }
}
