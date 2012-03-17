#include "socket.h"
#include "../core/common.h"
#include <iostream>
#include <cstring>
#if defined (WIN32)
    #include <winsock2.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <stdio.h>
    #include <arpa/inet.h>
    #define SOCKET_ERROR (-1)
#endif

namespace Epyx
{
    // Socket library initialisation
    int Socket::is_init = 0;
    Mutex Socket::init_mutex;

    void Socket::init(){
        if (is_init == 0){
            #if defined (WIN32)
                WSADATA WSAData;
                int error = WSAStartup(MAKEWORD(2,2), &WSAData);
            #endif
        }
        init_mutex.lock();
        is_init++;
        init_mutex.unlock();
    }

    void Socket::fini(){
        init_mutex.lock();
        is_init--;
        init_mutex.unlock();
        if (is_init == 0){
            #if defined (WIN32)
                WSACleanup();
            #endif
        }
    }

    Socket::Socket()
        :sock(-1), isConnected(false)
    {
    }

    Socket::Socket(const Address &addr)
        :sock(-1), isConnected(false), address(addr)
    {
    }

    Socket::Socket(const char* addr, unsigned short port)
        :sock(-1), isConnected(false), address(addr, port)
    {
    }

    Socket::Socket(int sock, const Address &addr)
        :sock(sock), isConnected(true), address(addr)
    {
        EPYX_ASSERT(sock >= 0);
    }

    Socket::~Socket()
    {
        this->close();
    }

    void Socket::setFd(int sockfd)
    {
        EPYX_ASSERT(this->sock < 0);
        EPYX_ASSERT(sockfd >= 0);
        this->sock = sockfd;
    }

    int Socket::getFd()
    {
        EPYX_ASSERT(sock >= 0);
        return sock;
    }

    void Socket::setAddress(Address& addr)
    {
        address = addr;
    }

    Address Socket::getAddress()
    {
        return address;
    }

    bool Socket::connect()
    {
        EPYX_ASSERT(this->sock >= 0);
        //TODO : Include Logger functionnality somewhere.
        sockaddr_storage server;
        this->address.getSockAddr((struct sockaddr*)&server);
        int result = ::connect(this->sock, (sockaddr*)&server, sizeof(server));
        if (result < 0) {
            //Replace by error log.
            Epyx::log::error << "Failed connecting to " << this->address << ": "
                << strerror(errno) << Epyx::log::endl;
            return false;
        }
        this->isConnected = true;
        return true;
    }

    void Socket::close(){ //TODO Implement the tests.
        //Epyx::log::debug << "Closing Socket ..." << Epyx::log::endl;
        //Is Socket connected? If so,
        if (sock < 0)
            return;
        if (isConnected)
            ::shutdown(sock,SHUT_RDWR);
        isConnected = false;
        ::close(sock);
        sock = -1;
        //Epyx::log::debug << "Socket Closed" << Epyx::log::endl;
    }

    /**
     * Send bytes through the network
     * return: number of sent bytes
     */
    int Socket::send(const void *data, int size)
    {
        int bytes;
        EPYX_ASSERT(data != NULL);
        EPYX_ASSERT(this->sock >= 0);
        EPYX_ASSERT(this->isConnected);
        bytes = ::send(this->sock, data, size, 0);
        // TODO: Implement status error (ex. Conn closed, ...)

        if (bytes == -1)
            throw ErrException("Socket", "send");
        return bytes;
    }

    /**
     * Send all bytes through the network
     * return: number of sent bytes
     */
    void Socket::sendAll(const void *data, int size)
    {
        int sentBytes;
        EPYX_ASSERT(data != NULL);
        while (size > 0) {
            sentBytes = this->send(data, size);
            if (!sentBytes)
                throw FailException("Socket", "Unable to send data");
            EPYX_ASSERT(sentBytes <= size);
            size -= sentBytes;
        }
    }

    /**
     * Send text through the network
     */
    void Socket::write(std::string message)
    {
        // The c_str contains one more character than the length of the string : \0
        // but we do not want to send \0 character through the network
        this->sendAll(message.c_str(), message.length());
    }

    /**
     * Receive some bytes from the network in a buffer
     * @data: buffer pointer
     * @size: buffer size
     * return: number of bytes received
     */
    int Socket::recv(void *data, int size)
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

    /**
     * Receive exactly (size) bytes from the network
     * @data: buffer pointer
     * @size: buffer size
     */
    void Socket::recvAll(void *data, int size)
    {
        int recvBytes;
        EPYX_ASSERT(data != NULL);
        // TODO: feof !
        while (size > 0) {
            recvBytes = this->recv(data, size);
            if (!recvBytes)
                throw FailException("Socket", "Unable to recv data");
            EPYX_ASSERT(recvBytes <= size);
            size -= recvBytes;
        }
    }

    /**
     * Receive a line from the network
     * @out: output stream where data are written
     * return: true if a line was written, false otherwise
     *
     * TODO: Use an internal buffer instead of read one character each time
     */
    bool Socket::recvLine(std::ostream& out)
    {
        char c;
        while (this->recv(&c, 1)) {
            // Socket may receive \r\n end-of-line
            if (this->last_eol == '\r' && c == '\n')
                continue;

            // Detect line end character
            if (c == '\n' || c == '\r') {
                this->last_eol = c;
                return true;
            }

            // Reset EOL if a normal character is received
            if (this->last_eol)
                this->last_eol = 0;

            out << c;
        }
        return false;
    }

    /**
     * Read a line into a string
     */
    std::string Socket::read()
    {
        std::ostringstream out;
        if (!this->recvLine(out))
            return "";
        return out.str();
    }
}
