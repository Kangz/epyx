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
    class ServerLink;
    class Thread;
    class Address;

    /**
     * This function is called each time the server accepts a connection
     * Returns true if ok, false tells the server to quit
     * Throw an exception onerror
     */
    typedef bool (ServerRun)(ServerLink& link);
    

    class ServerListener
    {
    private:
        int sockfd;
        struct addrinfo ai;
        Thread *thread;
        volatile bool running;
        Address *srvAddr;
    public:
        ServerRun *runFn;

        ServerListener();
        ~ServerListener();
        void run(struct addrinfo *pai, int nbConn, ServerRun runFn_);
        void AcceptLoop();
        void wait();
        void stop();
    };
}

#endif /* EPYX_SERVER_LISTENER_H */
