#ifndef EPYX_TCPSERVER_H
#define EPYX_TCPSERVER_H

#include "server.h"

namespace Epyx
{
    class TCPServer : public Server
    {
    public:
        TCPServer(unsigned short port, unsigned int nbConn, ServerRun& srvRun);
        ~TCPServer();

        bool bind();
    };

}
#endif // EPYX_TCPSERVER_H
