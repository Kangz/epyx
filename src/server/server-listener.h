/**
 * Server listener, in a separate thread
 */
#ifndef EPYX_SERVER_LISTENER_H
#define EPYX_SERVER_LISTENER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

namespace Epyx
{
    class Server;
    class Thread;
    
    class ServerListener
    {
    private:
        Server *srv;
        int sockfd;
        struct addrinfo ai;
        Thread *thread;
    public:
        ServerListener();
        ~ServerListener();
        void run(Server *srv_, struct addrinfo *pai, int nbConn);
        void AcceptLoop();
        void wait();
    };
}

#endif /* EPYX_SERVER_LISTENER_H */
