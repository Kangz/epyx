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

    ServerListener::ServerListener()
    {
        this->sockfd = -1;
        this->thread = NULL;
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

    void ServerListener::run(Server *srv_, struct addrinfo *pai, int nbConn)
    {
        int status, flag;
        
        if (!pai || !srv_)
            throw FailException("ServerListener", "pai or server is NULL");
        this->srv = srv_;
        this->ai = *pai;
        this->sockfd = -1;
        this->thread = NULL;

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

        this->thread = new Thread(server_runAcceptLoop, this);
        this->thread->run();
    }
            
    void ServerListener::AcceptLoop()
    {
        int newfd = -1;
        struct sockaddr_storage clientAddr;
        socklen_t clientAddrLen;
        ServerLink *newLink = NULL;
        Address srvAddr(this->ai.ai_addr);
        std::cout << "Listening on " << srvAddr << "\n";
        try {
            while (1) {
                clientAddrLen = sizeof clientAddr;
                newfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrLen);
                if (newfd == -1)
                    throw new ErrException("ServerListener", "accept");

                newLink = new ServerLink(this->ai.ai_addr, (struct sockaddr*)&clientAddr,
                                         newfd);

                // Here call something wise and smart
                if (send(newfd, "Hello, world!\n", 14, 0) == -1)
                    throw new ErrException("ServerListener", "send");
                
                close(newfd);
                newfd = -1;
            }
        } catch (Exception e) {
            std::cerr << e << "[Srv] Arg ! An exception killed me !\n";
            if (newLink)
                delete newLink;
            if (newfd)
                close(newfd);
        }
    }

    void ServerListener::wait()
    {
        if (this->thread)
            this->thread->wait();
    }
}
