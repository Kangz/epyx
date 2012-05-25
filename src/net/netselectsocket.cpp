#include "netselectsocket.h"
#include "../core/common.h"
#include "socket.h"

namespace Epyx
{

    NetSelectSocket::NetSelectSocket(Socket& sock)
    :sock(&sock), dynamicSock(false) {
    }

    NetSelectSocket::NetSelectSocket(Socket *sock)
    :sock(sock), dynamicSock(true) {
    }

    NetSelectSocket::~NetSelectSocket() {
        if (sock != NULL && dynamicSock) {
            delete sock;
            sock = NULL;
        }
    }

    int NetSelectSocket::getFileDescriptor() const {
        EPYX_ASSERT(sock != NULL);
        return sock->getFd();
    }

    bool NetSelectSocket::read() {
        EPYX_ASSERT(sock != NULL);
        const int size = 4096;
        char data[size];
        int recvSize = sock->recv(data, size);

        // Quit on End-Of-File
        if (recvSize == 0)
            return false;

        // Eat data
        try {
            this->eat(data, recvSize);
        } catch (MinorException e) {
            log::error << e << log::endl;
            log::error << "Closing socket to " << sock->getAddress() <<
                " due to an exception" << log::endl;
            return false;
        }
        return sock->isOpened();
    }

    Socket& NetSelectSocket::socket() const {
        EPYX_ASSERT(sock != NULL);
        return *sock;
    }
}
