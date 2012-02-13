/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * epyx
 * Copyright (C) aniem 2012 <jeremy.buet@polytechnique.org>
 *
 */

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

#include "exception.h"
#include "socket.h"
#include "assert.h"
#include <iostream>
#include <cstring>

namespace Epyx{
    #define BUF_SIZE 4096

    int Socket::is_init = 0;
    Mutex Socket::init_mutex;

    Socket::Socket(){
        initialize();
    }

    Socket::Socket(std::string add, unsigned short p) {
        initialize();
        this->setPort(p);
        this->setAddress(add);
    }

    Socket::~Socket(){
        close();
    }

    void Socket::initialize(){
        port = -1;
        this->sock = ::socket(AF_INET,SOCK_STREAM,0);
        if (this->sock == -1)
            ErrException("Socket","constructor");
    }
    
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
        static int erreur;
        init_mutex.lock();
        is_init--;
        init_mutex.unlock();
        if (is_init == 0){
            #if defined (WIN32)
                WSACleanup();
            #endif
        }
    }


    void Socket::setAddress(std::string add){
        address=std::string(add);
    }

    std::string Socket::getAddress(){
        return address;
    }

    void Socket::setPort(unsigned short p){
        port = p;
    }

    unsigned short Socket::getPort(){
        return port;
    }

    int Socket::connect(){ //TODO : Include Logger functionnality somewhere.
        sockaddr_in server;
        server.sin_family       = AF_INET;
        server.sin_addr.s_addr  = inet_addr(address.c_str());
        server.sin_port         = htons(port);
        memset(&server.sin_zero, '\0', sizeof(server.sin_zero));
        int result = ::connect(this->sock, (sockaddr *)&server, sizeof(server));
        if ( result < 0){
            std::cerr << "Failed connecting to " << address << ":" << port << " : " << strerror(errno) << std::endl; //Replace by error log.
            std::cerr << this->sock << "   " << server.sin_zero << std::endl;
            return -1;
        }
        return 0;
    }

    void Socket::close(){ //TODO Implement the tests.
        //std::cout << "Closing Socket ..." << std::endl;
        //Is Socket connected? If so,
        ::shutdown(sock,SHUT_RDWR);
        //Is Socket close? If so,
        ::close(sock);
        //std::cout << "Socket Closed" << std::endl;
    }

    /**
     * Send bytes through the network
     * return: number of sent bytes
     */
    int Socket::send(const void *data, int size)
    {
        int bytes;
        EPYX_ASSERT(data != NULL);
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
        int bytes;
        EPYX_ASSERT(data != NULL);
        while (size > 0) {
            bytes = this->send(data, size);
            if (!bytes)
                throw FailException("Socket", "Unable to send data");
            if (bytes > size)
                throw FailException("Socket", "More data was sent ?");
            size -= bytes;
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
        bytes = ::recv(this->sock, data, size-1, 0);
        //recv doesn't set the after-the-last byte to zero. We must do it to avoid some issues. (writing into a prefilled longer data buffer fucks everything up)
        //memset( ((char *) data) + bytes,'\0',1); //The only way to set it correctly (with data being a void pointer).  the char* is here to set correctly the pointer arithmetic (So it advances "bytes" bytes)
        *(((char *) data) + bytes) = '\0';
        // TODO: Implement status error (ex. Conn closed, ...)
        if (bytes == -1)
            throw ErrException("Socket", "recv");
        return bytes;
    }
    
    /**
     * Receive exactly (size) bytes from the network
     * @data: buffer pointer
     * @size: buffer size
     */
    void Socket::recvAll(void *data, int size)
    {
        int bytes;
        EPYX_ASSERT(data != NULL);
        // TODO: feof !
        while (size > 0) {
            bytes = this->recv(data, size);
            if (!bytes)
                throw FailException("Socket", "Unable to recv data");
            if (bytes > size)
                throw FailException("Socket", "More data was recv ?");
            size -= bytes;
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

