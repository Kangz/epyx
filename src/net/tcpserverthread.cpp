#include "tcpserverthread.h"
#include "../core/common.h"

namespace Epyx
{
    TCPServerThread::TCPServerThread(unsigned short port, unsigned int nbConn,
            ServerRun& srvRun, std::string name, int id)
    :TCPServer(port, nbConn), Thread(name, id), srvRun(srvRun)
    {
    }

    void TCPServerThread::run()
    {
        int newfd = -1;
        struct sockaddr_storage clientAddr;
        socklen_t clientAddrLen;
        Socket *newSock = NULL;
        ServerThread *sthread = NULL;
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
                        sthread = new ServerThread(this->srvRun, this, newSock,
                            "client", spawnId++);
                        sthread->start();
                    } catch (Exception e) {
                        log::error << e << log::endl;
                        log::error << "Unable to start a thread" << log::endl;
                        if (sthread != NULL)
                            delete sthread;
                        delete newSock;
                    }
                    // DO NOT delete newSock nor srun as they are owned by an other thread
                    newSock = NULL;
                    sthread = NULL;
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
