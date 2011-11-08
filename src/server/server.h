/**
 * Basic server
 */
#ifndef EPYX_SERVER_H
#define EPYX_SERVER_H

#include "server-listener.h" /* for ServerRun */

namespace Epyx
{
    class ServerLink;
    class ServerListener;

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

        Server(int port_, int nbConn_, ServerRun runFn);
        virtual ~Server();
    };
}

#endif /* EPYX_SERVER_H */
