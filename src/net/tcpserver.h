#ifndef EPYX_TCPSERVER_H
#define EPYX_TCPSERVER_H

#include "server.h"

namespace Epyx
{
    class TCPServer : public Server
    {
    public:
        TCPServer(unsigned short port, unsigned int nbConn);
        ~TCPServer();

        bool bind();

    private:
        unsigned int nbConn;
    };

}
#endif /* EPYX_TCPSERVER_H */
