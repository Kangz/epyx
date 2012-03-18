#include "netselectsocket.h"
#include "../core/common.h"
#include "socket.h"

namespace Epyx
{
    NetSelectSocket::NetSelectSocket(Socket& sock)
        :sock(&sock), dynamicSock(false)
    {
    }


    NetSelectSocket::NetSelectSocket(Socket *sock)
        :sock(sock), dynamicSock(true)
    {
    }

    NetSelectSocket::~NetSelectSocket()
    {
        if (sock != NULL && dynamicSock) {
            delete sock;
            sock = NULL;
        }
    }


    int NetSelectSocket::getFileDescriptor()
    {
        EPYX_ASSERT(sock != NULL);
        int fd = sock->getRecvFd();
        EPYX_ASSERT(fd >= 0);
        return fd;
    }

    bool NetSelectSocket::read()
    {
        EPYX_ASSERT(sock != NULL);
        const int size = 4096;
        char data[size];
        int recvSize = sock->recv(data, size);

        // Quit on End-Of-File
        if (recvSize == 0)
            return false;

        // Eat data
        this->eat(data, recvSize);
        return true;
    }

    Socket& NetSelectSocket::socket()
    {
        EPYX_ASSERT(sock != NULL);
        return *sock;
    }
}
