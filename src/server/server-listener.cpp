#include "server-listener.h"
#include <iostream>
#include "../core/exception.h"
#include "../core/thread.h"
#include "server.h"
#include "server-link.h"

namespace Epyx
{
    /**
     * Server listening thread
     */
    static void server_runAcceptLoop(void *arg)
    {
        if (!arg)
            std::cerr << "[server_runAcceptLoop] Err: arg = NULL\n";
        else {
            ServerListener *self = (ServerListener*)arg;
            self->AcceptLoop();
        }
    }

    /**
     * Server accepting thread
     */
    static void server_runAcceptFn(void *arg)
    {
        bool running = true;
        if (!arg) {
            std::cerr << "[server_runAcceptFn] Err: arg = NULL\n";
            return;
        }
        ServerLink *self = (ServerLink*)arg;
        std::cout << "[Srv] >> " << self->clientAddr
            << " Incoming for " << self->srvAddr << "\n";

        try {
            // This code is quite horrible for a C programmer
            running = self->listener->runFn(*self);
            if (!running)
                self->listener->stop();
            std::cout << "[Srv] << " << self->clientAddr <<
                " Client terminated\n";
            self->close();
        } catch (Exception e) {
            std::cerr << "[Srv] >< " << self->clientAddr
                << " Error:\n" << e << "\n";
        }
        delete self;
    }

    ServerListener::ServerListener()
    {
        this->sockfd = -1;
        this->thread = NULL;
        this->running = false;
    }
    
    ServerListener::~ServerListener()
    {
        if (this->sockfd >= 0) {
            close(this->sockfd);
            this->sockfd = -1;
        }
        if (this->thread) {
            delete this->thread;
            this->thread = NULL;
        }
    }

    void ServerListener::run(struct addrinfo *pai, int nbConn, ServerRun runFn_)
    {
        int status, flag;
        
        if (!pai)
            throw FailException("ServerListener", "pai is NULL");
        this->sockfd = -1;
        this->thread = NULL;
        this->runFn = runFn_;
        this->running = false;

        // Create a new socket
        this->sockfd = socket(pai->ai_family, pai->ai_socktype, pai->ai_protocol);
        if (this->sockfd < 0)
            throw ErrException("ServerListener", "socket");

        // Allow reusing of the address
        flag = 1;
        status = setsockopt (this->sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
        if (status != 0)
            throw ErrException("ServerListener", "setsockopt");

        // Bind to a listening address/port
        if ((status = bind(sockfd, pai->ai_addr, pai->ai_addrlen)) < 0) {
            close(this->sockfd);
            this->sockfd = -1;
            throw ErrException("ServerListener", "bind");
        }

        // Listening to nbConn connections
        if ((status = listen(sockfd, nbConn))  < 0) {
            close(this->sockfd);
            this->sockfd = -1;
            throw ErrException("ServerListener", "listen");
        }

        this->srvAddr = new Address(pai->ai_addr);

        this->thread = new Thread(server_runAcceptLoop, this);
        this->thread->run();
    }
            
    void ServerListener::AcceptLoop()
    {
        int newfd = -1;
        bool running = true; 
        struct sockaddr_storage clientAddr;
        socklen_t clientAddrLen;
        ServerLink *newLink = NULL;
        std::cout << "[Srv] Listening on " << this->srvAddr << "\n";
        try {
            this->running = true;
            while (this->running && this->sockfd >= 0) {
                clientAddrLen = sizeof clientAddr;
                newfd = accept(this->sockfd, (struct sockaddr*)&clientAddr,
                               &clientAddrLen);
                if (newfd == -1)
                    throw ErrException("ServerListener", "accept");

                // If this thread was not killed, return
                if (!this->running) {
                    close(newfd);
                    break;
                }

                try {
                    newLink = new ServerLink(this, srvAddr,
                                             (struct sockaddr*)&clientAddr,
                                             newfd);
                } catch (Exception e) {
                    std::cerr << "[Srv] Unable to setup the link:\n" << e << "\n";
                    if (newLink)
                        delete newLink;
                    else if (newfd >= 0) {
                        // newfd is managed by newLink
                        close(newfd);
                    }
                    newLink = NULL;
                }
                newfd = -1;

                if (newLink != NULL) {
                    try {
                        Thread t(server_runAcceptFn, newLink);
                        t.run();
                    } catch (Exception e) {
                        std::cerr << "[Srv] Unable to start a thread:\n" << e << "\n";
                        delete newLink;
                    }
                    // DO NOT delete newLink; it is owned by a thread
                    newLink = NULL;
                }
            }
        } catch (Exception e) {
            std::cerr << e << "[Srv] Arg ! An exception killed me !\n";
            if (newLink)
                delete newLink;
        }
        std::cout << "[Server] Clean exit on " << srvAddr << "\n";
    }

    void ServerListener::wait()
    {
        if (this->thread)
            this->thread->wait();
    }

    void ServerListener::stop()
    {
        if (this->thread)
            this->thread->term();
        this->running = false;
    }
}
