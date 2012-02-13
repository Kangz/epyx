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
#include "mutex.h"

namespace Epyx{
    class Socket
    {
    public:
        int connect();
        Socket();
        static void init();
        void initialize();
        static void fini();
        Socket(std::string address, unsigned short port);
        void setAddress(std::string address);
        void setPort(unsigned short port);
        unsigned short getPort();
        std::string getAddress();
        void close();
        int send(const void *data, int size);
        void sendAll(const void *data, int size);
        void write(std::string message);
        int recv(void *data, int size);
        void recvAll(void *data, int size);
        bool recvLine(std::ostream& out);
        std::string read();
        ~Socket();
    protected:
        int sock;
        
    private:
        unsigned short port;
        std::string address;
        static int is_init;
        static Mutex init_mutex;
        // Last end-of-line caracter (\r or \n)
        char last_eol;

    //    std::string outBuffer;
    //    std::string inBuffer;
    };
}
#endif // EPYX_SOCKET_H_
