#include "tcpsocket.h"
#include "../core/common.h"

namespace Epyx{
    TCPSocket::TCPSocket()
    {
        this->sock = ::socket(AF_INET, SOCK_STREAM, 0);
        if (this->sock == -1)
            throw ErrException("TCPSocket", "socket");
    }
    TCPSocket::TCPSocket(const Address& addr)
        :Socket(addr)
    {
        this->sock = ::socket(AF_INET, SOCK_STREAM, 0);
        if (this->sock == -1)
            throw ErrException("TCPSocket", "socket");
    }
    TCPSocket::TCPSocket(int sock, const Address &addr)
        :Socket(sock, addr)
    {
    }

    int TCPSocket::getRecvFd()
    {
        return this->getFd();
    }
}
