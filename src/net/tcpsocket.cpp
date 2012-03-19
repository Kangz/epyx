#include "tcpsocket.h"
#include "../core/common.h"

namespace Epyx{
    TCPSocket::TCPSocket()
        :isConnected(false)
    {
    }
    TCPSocket::TCPSocket(const Address& addr)
        :Socket(addr),isConnected(false)
    {
    }
    TCPSocket::TCPSocket(int sock, const Address &addr)
        :Socket(sock, addr), isConnected(true)
    {
    }
    bool TCPSocket::connect()
    {
        if (this->sock == -1){
            this->sock = ::socket(AF_INET, SOCK_STREAM, 0);
            if (this->sock == -1) {
                log::error << "Failed TCPSocket::socket to " << this->address << ": "
                    << log::errstd << log::endl;
                return false;
            }
        }
        sockaddr_storage server;
        this->address.getSockAddr((struct sockaddr*)&server);
        int result = ::connect(this->sock, (sockaddr*)&server, sizeof(server));
        if (result < 0) {
            //Replace by error log.
            log::error << "Failed connecting to " << this->address << ": "
                << log::errstd << log::endl;
            return false;
        }
        this->isConnected = true;
        return true;
    }
    /**
     * Send bytes through the network
     * return: number of sent bytes
     */
    int TCPSocket::send(const void *data, int size)
    {
        int bytes;
        EPYX_ASSERT(data != NULL);
        if (!this->isConnected)
            this->connect();
        EPYX_ASSERT(this->sock >= 0);
        EPYX_ASSERT(this->isConnected);
        bytes = ::send(this->sock, data, size, 0);
        // TODO: Implement status error (ex. Conn closed, ...)

        if (bytes == -1)
            throw ErrException("Socket", "send");
        return bytes;
    }
    /**
     * Receive some bytes from the network in a buffer
     * @data: buffer pointer
     * @size: buffer size
     * return: number of bytes received
     */
    int TCPSocket::recv(void *data, int size)
    {
        int bytes;
        EPYX_ASSERT(data != NULL);
        EPYX_ASSERT(this->sock >= 0);
        EPYX_ASSERT(this->isConnected);
        bytes = ::recv(this->sock, data, size, 0);
        // TODO: Implement status error (eg. Conn closed, ...)
        if (bytes == -1)
            throw ErrException("Socket", "recv");

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
