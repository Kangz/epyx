#include "tcpserver.h"
#include "../core/common.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

namespace Epyx
{
    TCPServer::TCPServer(unsigned short port, unsigned int nbConn, ServerRun& srvRun)
        :Server(port), nbConn(nbConn), srvRun(srvRun)
    {
    }

    TCPServer::~TCPServer()
    {
    }

    bool TCPServer::bind()
    {
        if (!this->_internal_bind(SOCK_STREAM))
            return false;

        // Now there is a socket, listen to nbConn connections
        EPYX_ASSERT(sockfd >= 0);
        int status = listen(sockfd, nbConn);
        if (status < 0) {
            log::error << "listen: " << log::errstd << log::endl;
            this->close();
            return false;
        }
        return true;
    }

    void TCPServer::run()
    {
        int newfd = -1;
        struct sockaddr_storage clientAddr;
        socklen_t clientAddrLen;
        Socket *newSock = NULL;
        ServerRunnable *srun = NULL;
        int spawnId = 1;

        // Bind if no socket available
        if (this->sockfd < 0 && !this->bind())
            throw FailException("TCPServer::run", "Unable to bind");

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
                    throw ErrException("TCPServer::run", "accept");

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
