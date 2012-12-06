#include "listener.h"

namespace Epyx
{
    namespace DirectConnection
    {

        Listener::Listener(TCPServer *srv)
        :srv(srv), hasAccepted(false) {
        }

        int Listener::getFileDescriptor() const {
            EPYX_ASSERT(srv);
            return srv->getFd();
        }

        bool Listener::read() {
            EPYX_ASSERT(srv);
            std::unique_ptr<TCPSocket> newSock = srv->accept();
            sock.swap(newSock);
            hasAccepted = true;
            acceptCond.notify_all();

            // Stop listening
            return false;
        }

        std::unique_ptr<TCPSocket> Listener::waitForAccept(int msec) {
            while (!hasAccepted) {
                std::mutex m;
                std::unique_lock<std::mutex> lock(m);
                if (acceptCond.wait_for(lock, std::chrono::milliseconds(msec)) == std::cv_status::timeout) {
                    // Timeout
                    std::unique_ptr<TCPSocket> emptyRes;
                    return emptyRes;
                }
            }
            return std::move(sock);
        }

        SockAddress Listener::getListenAddress() const {
            EPYX_VERIFY(srv);
            return srv->getAddress();
        }
    }
}
