#include "netselectsocket.h"
#include "../core/common.h"
#include "socket.h"

namespace Epyx
{
    NetSelectSocket::NetSelectSocket(Socket& sock)
        :sock(sock)
    {
    }

    bool NetSelectSocket::read()
    {
        const int size = 4096;
        char data[size];
        int s;
        s = sock.recv(data, size);

        // Quit on End-Of-File
        if (s == 0)
            return false;

        // Eat data
        this->eat(data, size);
        return true;
    }
}
