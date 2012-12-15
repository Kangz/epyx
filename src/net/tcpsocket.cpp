#include "tcpsocket.h"
#include "../core/common.h"

namespace Epyx
{

    TCPSocket::TCPSocket() {
    }

    TCPSocket::TCPSocket(const SockAddress& addr)
    :Socket(addr) {
        this->connect();
    }

    TCPSocket::TCPSocket(int sock, const SockAddress &addr)
    :Socket(sock, addr) {
    }

    bool TCPSocket::connect(bool force) {
        // force indicates to call connect even if we already have a socket
        if (!force && this->sock >= 0) {
            return true;
        }
        if (this->sock == -1) {
            this->sock = ::socket(AF_INET, SOCK_STREAM, 0);
            if (this->sock == -1) {
                log::error << "Unable to create a new TCPSocket to " <<
                    this->address << ": " << log::errstd << log::endl;
                return false;
            }
        }
        sockaddr_storage server;
        this->address.getSockAddr((struct sockaddr*) &server);
        int result = ::connect(this->sock, (struct sockaddr*) &server, sizeof (server));
        if (result < 0) {
            //Replace by error log.
            log::error << "Failed connecting to " << this->address << ": "
                << log::errstd << log::endl;
            this->close();
            EPYX_ASSERT(this->sock < 0);
            return false;
        }
        this->updateLocalAddress();
        return true;
    }

    int TCPSocket::send(const void *data, int size) {
        int bytes;
        EPYX_ASSERT(data != NULL);
        if (this->sock < 0 && !this->connect()) {
            return 0;
        }
        EPYX_ASSERT(this->sock >= 0);
        bytes = ::send(this->sock, data, size, 0);
        if (bytes == -1) {
            // Verbose error and close socket
            log::error << "TCPSocket::send error with " << address << ": "
                << log::errstd << log::endl;
            this->close();
            return 0;
        }
        return bytes;
    }

    int TCPSocket::recv(void *data, int size) {
        int bytes;
        EPYX_ASSERT(data != NULL);
        if (this->sock < 0 && !this->connect()) {
            return 0;
        }
        EPYX_ASSERT(this->sock >= 0);
        bytes = ::recv(this->sock, data, size, 0);
        if (bytes == -1) {
            // Verbose error and close socket
            log::error << "TCPSocket::recv error from " << address << ": "
                << log::errstd << log::endl;
            this->close();
            return 0;
        }

        /**
         * recv doesn't set the after-the-last byte to zero. We must do it to
         * avoid some issues.
         * (writing into a prefilled longer data buffer fucks everything up)
         */
        if (bytes < size)
            ((char*) data)[bytes] = 0;
        return bytes;
    }


}
