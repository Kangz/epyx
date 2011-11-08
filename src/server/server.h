/**
 * Basic server
 */
#ifndef EPYX_SERVER_H
#define EPYX_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "../core/thread.h"
#include "../core/address.h"

namespace Epyx
{
    class Server;

    /**
     * Link data
     */
    class ServerLink
    {
    private:
        Address clientAddr;
        Address srvAddr;
        int sockfd;
    public:
        ServerLink(struct sockaddr *aiSrv, struct sockaddr *aiClient, int sockfd_);
    };
    
    /**
     * Listener data
     */
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
        
    class Server
    {
    private:
        /**
         * TCP Port
         */
        int port;

        /**
         * Max number of connections
         */
        int nbConn;

        /**
         * number of listening threads
         */
        int nbListener;
        ServerListener *listener;
        
    public:
        /**
         * Global setup
         */
        static bool init();

        Server(int port_, int nbConn_);
        ~Server();
    };
}

#endif /* EPYX_SERVER_H */
