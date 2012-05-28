#include "udpsocket.h"
#include "../core/common.h"

namespace Epyx
{

    UDPSocket::UDPSocket() {
    }

    UDPSocket::UDPSocket(const Address& addr)
    :Socket(addr) {
    }

    UDPSocket::UDPSocket(int sock, const Address &addr)
    :Socket(sock, addr) {
    }

    int UDPSocket::send(const void *data, int size) {
        int bytes;
        struct sockaddr_storage saddr;

        EPYX_ASSERT(data != NULL);
        // Create a new socket if it does not exist
        if (this->sock < 0) {
            this->sock = ::socket(AF_INET, SOCK_DGRAM, 0);
            if (this->sock == -1)
                throw ErrException("UDPSocket", "socket");
        }
        address.getSockAddr((struct sockaddr *) &saddr);
        bytes = ::sendto(this->sock, data, size, 0, (const struct sockaddr *) &saddr, sizeof (saddr));
        if (localAddress.empty())
            this->updateLocalAddress();
        // TODO: Implement status error (ex. Conn closed, ...)

        if (bytes < 0)
            throw ErrException("UDPSocket", "sendto");
        return bytes;
    }

    int UDPSocket::recv(void * data, int size) {
        int bytes;
        struct sockaddr_storage saddr;
        socklen_t length = sizeof (saddr);
        EPYX_ASSERT(data != NULL);
        EPYX_ASSERT(this->sock >= 0);
        bytes = ::recvfrom(this->sock, data, size, 0, (struct sockaddr *) &saddr, &length);
        // TODO: Implement status error (eg. Conn closed, ...)
        if (bytes < 0)
            throw ErrException("UDPSocket", "recvfrom");

        /**
         * recv doesn't set the after-the-last byte to zero. We must do it to
         * avoid some issues.
         * (writing into a prefilled longer data buffer fucks everything up)
         */
        if (bytes < size)
            ((char*) data)[bytes] = 0;
        lastRecvAddr = Address((const sockaddr *) &saddr);
        return bytes;
    }

    Address UDPSocket::getLastRecvAddr() const {
        return lastRecvAddr;
    }

}
