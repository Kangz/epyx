/**
 * Runnable object which manage a server connection
 * ServerRunnable is an internal class to use ServerRun with a thread.
 */
#ifndef EPYX_SERVERRUN_H
#define EPYX_SERVERRUN_H

#include "socket.h"
#include "../core/thread.h"

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
     * The ServerThread is intended to pass parameters to a ServerRun object
     */
    class ServerThread : public Thread
    {
    public:
        ServerThread(ServerRun& srvRun, Server *srv, Socket *sock,
            std::string name, int id);
        void run();
    private:
        ServerRun& srvRun;
        Server *srv;
        Socket *sock;
    };
}

#endif // EPYX_SERVERRUN_H
