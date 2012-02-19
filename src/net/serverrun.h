/**
 * Runnable object which manage a server connection.
 */
#ifndef EPYX_SERVERRUN_H
#define EPYX_SERVERRUN_H

#include "socket.h"
#include "../core/runnable.h"

namespace Epyx
{
    class Server;

    class ServerRun
    {
    public:
        /**
         * This function is called once per connection but is common for EVERY
         * connections made to the server, so be careful because object
         * properties are shared with other threads.
         */
        virtual void srvrun(Server& srv, Socket& sock) = 0;
    };

    /**
     * The ServerRunnable is intended to pass parameters to a ServerRun object
     */
    class ServerRunnable : public Runnable
    {
    public:
        ServerRunnable(ServerRun& srvRun, Server *srv, Socket *sock);
        void run();
    private:
        ServerRun& srvRun;
        Server *srv;
        Socket *sock;
    };
}

#endif // EPYX_SERVERRUN_H
