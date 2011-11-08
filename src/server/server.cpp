#include "server.h"
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "../core/exception.h"

extern "C"
{
    /**
     * When a child is dead, the parent must wait() him
     */
    static void sigchld_handler(int s)
    {
        while(waitpid(-1, NULL, WNOHANG) > 0) {
            /* There were dead children */
        }
    }

}

namespace Epyx
{
    ServerLink::ServerLink(struct sockaddr *aiSrv, struct sockaddr *aiClient, int sockfd_)
        :srvAddr(aiSrv), clientAddr(aiClient), sockfd(sockfd_)
    {
        std::cout << ">> Incoming from " << clientAddr << " for " << srvAddr << "\n";
    }
    
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

    bool Server::init()
    {
        struct sigaction sigAct;
        
        // Dead process need to be waited
        sigAct.sa_handler = sigchld_handler;
        sigemptyset(&sigAct.sa_mask);
        sigAct.sa_flags = SA_RESTART;
        if (sigaction(SIGCHLD, &sigAct, NULL) == -1)
            throw ErrException("Server", "sigaction");

        return true;
    }

    Server::Server(int port_, int nbConn_)
    {
        struct addrinfo hints, *addrAll, *pai;
        int status, i;
        std::stringstream portSStream;

        if (port_ >= 65536)
            throw new FailException("Server", "Too high port");
        
        this->port = port_;
        this->nbConn = nbConn_;
        this->nbListener = 0;
        this->listener = NULL;

        portSStream << port_;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force IP version
        hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
        hints.ai_flags = AI_PASSIVE; // Server
        if ((status = getaddrinfo(NULL, portSStream.str().c_str(), &hints,
                                  &addrAll)) != 0) {
            FailException e("Server", "getaddrinfo error: ");
            e.append(gai_strerror(status));
            throw e;
        }

        // Count the results
        for (i = 0, pai = addrAll; pai != NULL; pai = pai->ai_next)
            i++;
        this->nbListener = i;
        if (!i) {
            std::cerr << "server: No listening opportunities found for "
                << port_ << " :-(\n";
            freeaddrinfo(addrAll);
            return;
        }
        this->listener = new ServerListener[i];
        for (i = 0, pai = addrAll; pai != NULL; pai = pai->ai_next, i++) {
            try {
                this->listener[i].run(this, pai, nbConn_);
                // Only run the first one
                break;
            } catch (Exception e) {
                std::cerr << e << "\n";
            }
        }
        freeaddrinfo(addrAll);

        // Wait until everybody has finished
        for (i = 0; i < this->nbListener; i++)
            this->listener[i].wait();
    }

    Server::~Server()
    {
        if (this->nbListener && this->listener) {
            this->nbListener = 0;
            delete[] this->listener;
            this->listener = NULL;
        }
    }
}
