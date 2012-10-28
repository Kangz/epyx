#include "listener.h"

namespace Epyx
{
    namespace DirectConnection
    {

        Listener::Listener(TCPServer *srv)
        :srv(srv), hasAccept(false) {
        }

        void Listener::run() {
            struct sockaddr_storage clientAddr;
            socklen_t clientAddrLen;

            clientAddrLen = sizeof clientAddr;

            int newfd = srv->getFd();

            ::accept(newfd, (struct sockaddr*) &clientAddr, &clientAddrLen);
            hasAccept = true;
            sock.reset(new TCPSocket(newfd, SockAddress((struct sockaddr*) &clientAddr)));
        }

        SockAddress Listener::getAddress() {
            return sock ? sock->getAddress() : SockAddress();
        }

        bool Listener::hasAccepted() {
            return this->hasAccept;
        }

        SockAddress Listener::getLocalAddress() {
            sock->updateLocalAddress();
            return sock->getLocalAddress();
        }

        std::unique_ptr<TCPSocket>& Listener::getSocket() {
            return sock;
        }
    } // namespace DirectConnection
} // namespace Epyx
