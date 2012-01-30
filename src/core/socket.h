/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * epyx
 * Copyright (C) aniem 2012 <jeremy.buet@polytechnique.org>
 *
 */

#ifndef EPYX_SOCKET_H_
#define EPYX_SOCKET_H_

#include <sys/socket.h>
#include <stdio.h>
#include <string>
namespace Epyx{
    class Socket
    {
    public:
        int connect();
        Socket();
        static void init();
        static void fini();
        Socket(std::string address, short port);
        void setAddress(std::string address);
        void setPort(short port);
        short getPort();
        std::string getAddress();
        int close();
        int write(std::string message);
        std::string recv();
        ~Socket();
    protected:

    private:
        static int erreur;
        std::string Address;
        short port;

        int socket;

    //    std::string outBuffer;
    //    std::string inBuffer;
    };
}
#endif // EPYX_SOCKET_H_
