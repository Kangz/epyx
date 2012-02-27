#ifndef EPYX_TCPSERVERTHREAD_H
#define EPYX_TCPSERVERTHREAD_H

#include "tcpserver.h"

namespace Epyx
{
    class TCPServerThread : public TCPServer, public Thread {
    public:
        TCPServerThread(unsigned short port, unsigned int nbConn,
            ServerRun& srvRun, std::string name, int id = -1);

    protected:
        /**
         * @brief Run a basic listening loop
         */
        void run();
    private:
        ServerRun& srvRun;
    };
}

#endif /* EPYX_TCPSERVERTHREAD_H */

