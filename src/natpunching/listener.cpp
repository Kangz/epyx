#include "listener.h"

namespace Epyx
{
    namespace DirectConnection
    {

        Listener::Listener(TCPServer *srv)
        :srv(srv), hasAccept(false),
        running_thread(&Listener::run, this) {
        }

        Listener::~Listener() {
            running_thread.join();
        }

        void Listener::run() {
            Thread::setName("OpenConnect::Listener");
            EPYX_VERIFY(srv);
            std::unique_ptr<TCPSocket> newSock = srv->accept();
            hasAccept = true;
            sock.swap(newSock);
        }

        std::unique_ptr<TCPSocket> Listener::retrieveSocket() {
            return std::move(sock);
        }

        bool Listener::hasAccepted() {
            return this->hasAccept;
        }

        SockAddress Listener::getListenAddress() const {
            EPYX_VERIFY(srv);
            return srv->getAddress();
        }
    }
}
