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
        :sock(-1)
    {
    }

    Socket::Socket(const Address &addr)
        :sock(-1), address(addr)
    {
    }

    Socket::Socket(const char* addr, unsigned short port)
        :sock(-1), address(addr, port)
    {
    }

    Socket::Socket(int sock, const Address &addr)
        :sock(sock), address(addr)
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

    bool Socket::isOpened()
    {
        return (sock >= 0);
    }

    void Socket::setAddress(const Address& addr)
    {
        address = addr;
    }

    Address Socket::getAddress()
    {
        return address;
    }

    void Socket::setLocalAddress(const Address& addr)
    {
        localAddress = addr;
    }

    Address Socket::getLocalAddress()
    {
        return localAddress;
    }

    void Socket::close(){
        //TODO Implement sanity checks on returned values of shutdown and close.
        // Do nothing if the socket is already closed
        if (sock < 0)
            return;
        ::shutdown(sock,SHUT_RDWR);
        ::close(sock);
        sock = -1;
    }

    bool Socket::sendAll(const void *data, int size)
    {
        int sentBytes;
        EPYX_ASSERT(data != NULL);
        while (size > 0) {
            sentBytes = this->send(data, size);
            if (!sentBytes) {
                log::error << "Socket::sendAll was unable to send " << size <<
                    " bytes" << log::endl;
                return false;
            }
            EPYX_ASSERT(sentBytes <= size);
            size -= sentBytes;
        }
        return true;
    }

    bool Socket::write(std::string message)
    {
        // Do not send \0 character through the network
        return this->sendAll(message.c_str(), message.length());
    }

    bool Socket::recvAll(void *data, int size)
    {
        int recvBytes;
        EPYX_ASSERT(data != NULL);
        // TODO: stop on feof !
        while (size > 0) {
            recvBytes = this->recv(data, size);
            if (!recvBytes) {
                log::error << "Socket::recvAll was unable to receive " << size <<
                    " bytes" << log::endl;
                return false;
            }
            EPYX_ASSERT(recvBytes <= size);
            size -= recvBytes;
        }
        return true;
    }
}
