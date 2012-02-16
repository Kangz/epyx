/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * epyx
 * Copyright (C) aniem 2012 <jeremy.buet@polytechnique.org>
 *
 */

#ifndef EPYX_SOCKET_H_
#define EPYX_SOCKET_H_

//#include <sys/socket.h>
#include <string>
#include "../core/mutex.h"
#include "address.h"

namespace Epyx
{
    class Socket
    {
    public:
        static void init();
        static void fini();

        Socket();
        Socket(Address& addr);
        Socket(const char* addr, unsigned int port);
        ~Socket();

        bool connect();
        void close();

        void setAddress(Address& addr);
        Address getAddress();

        // Senders
        int send(const void *data, int size);
        void sendAll(const void *data, int size);
        void write(std::string message);

        // Recevers
        int recv(void *data, int size);
        void recvAll(void *data, int size);
        bool recvLine(std::ostream& out);
        std::string read();

    protected:
        // Socket file descriptor
        int sock;
        bool isConnected;

        // IP + port associated with the socket
        Address address;

    private:
        // Global initialisation stuff
        static int is_init;
        static Mutex init_mutex;

        // Last end-of-line caracter (\r or \n)
        char last_eol;

    //    std::string outBuffer;
    //    std::string inBuffer;
    };
}
#endif // EPYX_SOCKET_H_
