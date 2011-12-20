#include "server.h"
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "../core/exception.h"
#include "server-listener.h"
#include "server-link.h"

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

    Server::Server(int port_, int nbConn_, ServerRun runFn)
    {
        struct addrinfo hints, *addrAll, *pai;
        int status, i;
        std::stringstream portSStream;

        if (port_ >= 65536)
            throw FailException("Server", "Too high port");

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
                this->listener[i].run(pai, nbConn_, runFn);
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
