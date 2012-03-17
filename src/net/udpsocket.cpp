#include "udpsocket.h"
#include "udpserver.h"
#include "../core/common.h"

namespace Epyx
{
    UDPSocket::UDPSocket(bool reply)
        :Socket("239.255.255.250", 0), answerServ(NULL)
    {
        // Create a new UDP socket
        this->sock = ::socket(PF_INET, SOCK_DGRAM, 0);
        if (this->sock == -1)
            throw ErrException("UDPSocket", "socket");

        // Create a UDP server
        if (reply) {
            answerServ = new UDPServer(this->address.getPort());
        }
    }

    bool UDPSocket::connect(){
        if (!Socket::connect())
            return false;
        if (answerServ != NULL){
            answerServ->setPort(address.getPort());
            if (!answerServ->bind())
                return false;
        }
        return true;
    }

    void UDPSocket::close(){
        Socket::close();
        if (answerServ != NULL){
            answerServ->close();
        }
    }

    int UDPSocket::recv(void *data, int size) {
        int bytes;
        EPYX_ASSERT(answerServ != NULL);
        EPYX_ASSERT(data != NULL);
        bytes = answerServ->recv(data, size);
        // TODO: Implement status error (ex. Conn closed, ...)
        return bytes;
    }

    int UDPSocket::getRecvFd()
    {
        EPYX_ASSERT(answerServ != NULL);
        return answerServ->getFd();
    }
}
