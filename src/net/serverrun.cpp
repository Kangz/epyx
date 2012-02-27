#include "serverrun.h"
#include "server.h"
#include "../core/log.h"

namespace Epyx
{
    ServerThread::ServerThread(ServerRun& srvRun, Server *srv, Socket *sock,
        std::string name, int id)
        :Thread(name, id), srvRun(srvRun), srv(srv), sock(sock)
    {
        EPYX_ASSERT(sock != NULL && srv != NULL);
    }

    void ServerThread::run()
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
        // FIXME: change this !
        delete this;
    }
}
