#ifndef EPYX_TCPSERVER_H
#define EPYX_TCPSERVER_H

#include "server.h"

namespace Epyx
{
    class TCPServer : public Server, public Runnable
    {
    public:
        TCPServer(unsigned short port, unsigned int nbConn, ServerRun& srvRun);
        ~TCPServer();

        bool bind();

        /**
         * Run a basic listening loop
         */
        void run();

    private:
        unsigned int nbConn;
        ServerRun& srvRun;
    };

}
#endif /* EPYX_TCPSERVER_H */
