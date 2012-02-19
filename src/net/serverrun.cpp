#include "serverrun.h"
#include "server.h"
#include "../core/log.h"

namespace Epyx
{
    ServerRunnable::ServerRunnable(ServerRun& srvRun, Server *srv, Socket *sock)
        :srvRun(srvRun), srv(srv), sock(sock)
    {
        EPYX_ASSERT(sock != NULL && srv != NULL);
    }

    void ServerRunnable::run()
    {
        EPYX_ASSERT(sock != NULL && srv != NULL);
        try {
            // Call real function
            this->srvRun.srvrun(*srv, *sock);
            log::debug << "Client terminated (was " << sock->getAddress()
                << ")" << log::endl;
            delete sock;
            sock = NULL;
        } catch (Exception e) {
            log::error << "Error from " << sock->getAddress()
                << " : " << e << log::endl;
        }
        // Close socket
        delete sock;
        // Yes, we can do it !
        delete this;
    }
}
