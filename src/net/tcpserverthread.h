#ifndef EPYX_TCPSERVERTHREAD_H
#define EPYX_TCPSERVERTHREAD_H

#include "../core/thread.h"
#include "tcpserver.h"
#include "socket.h"

namespace Epyx
{
    class TCPServerThread : public TCPServer, public Thread {
    public:
        TCPServerThread(unsigned short port, unsigned int nbConn);

        /**
         * @brief communicate with a client
         *
         * This function is called for each accepted connection
         */
        virtual void runSocket(Socket& sock) = 0;

    protected:
        /**
         * @brief Run a basic listening loop
         */
        void run();
    };
}

#endif /* EPYX_TCPSERVERTHREAD_H */

