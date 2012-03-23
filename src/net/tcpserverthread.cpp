#include "tcpserverthread.h"
#include "../core/common.h"
#include "tcpsocket.h"

namespace Epyx
{
    /**
     * @class Internal thread class to spawn a new thread for each accepted connection
     */
    class _TCPSocketThread : public Thread
    {
    public:
        _TCPSocketThread(TCPServerThread *srv, Socket *sock,
            std::string name, int id)
        :Thread(name, id), srv(srv), sock(sock)
        {
            EPYX_ASSERT(sock != NULL && srv != NULL);
        }
    protected:
        void run()
        {
            EPYX_ASSERT(sock != NULL && srv != NULL);
            try {
                // Call real function
                srv->runSocket(*sock);
                log::debug << "Client terminated (was " << sock->getAddress()
                    << ")" << log::endl;
                delete sock;
                sock = NULL;
            } catch (Exception e) {
                log::error << "Error from " << sock->getAddress()
                    << " : " << e << log::endl;
            }
            // Close socket
            delete sock;
            // Yes, we can do it !
            // FIXME: change this !
            delete this;
        }
    private:
        TCPServerThread *srv;
        Socket *sock;
    };

    TCPServerThread::TCPServerThread(unsigned short port, unsigned int nbConn)
    :TCPServer(port, nbConn)
    {
    }

    void TCPServerThread::run()
    {
        int newfd = -1;
        struct sockaddr_storage clientAddr;
        socklen_t clientAddrLen;
        Socket *newSock = NULL;
        _TCPSocketThread *sthread = NULL;
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
                    newSock = new TCPSocket(newfd, Address((struct sockaddr*)&clientAddr));
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
                        sthread = new _TCPSocketThread(this, newSock,
                            this->getThisName(), spawnId++);
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
