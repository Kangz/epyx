/**
 * Basic Internet server class
 */
#ifndef EPYX_SERVER_H
#define EPYX_SERVER_H

#include "address.h"
#include "serverrun.h"

namespace Epyx
{
    class Server : public Runnable
    {
    public:
        Server(unsigned short port, unsigned int nbConn, ServerRun& srvRun);
        ~Server();

        const Address& getAddress();

        /**
         * Start the server (socket + bind + listen)
         */
        virtual bool bind() = 0;

        /**
         * Stop the server (shutdown + close)
         */
        void close();

        /**
         * Run a basic listening loop
         */
        void run();

    protected:
        ServerRun& srvRun;
        Address address;
        unsigned short port;
        unsigned int nbConn;
        int sockfd;
        // Another thread tells the server to shut down
        bool running;

    private:
    };

}
#endif // EPYX_SERVER_H
