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

#include "socket.h"
#include <iostream>
#include <cstring>

namespace Epyx{
    #define BUF_SIZE 4096

    Socket::Socket(){
        //outBuffer ="";
        //inBuffer = "";
        address ="";
        port = -1;
        sock = ::socket(PF_INET,SOCK_STREAM,0);
        init();
    }

    Socket::Socket(std::string add, short p){
        Socket();
        address = add;
        port = p;
    }

    Socket::~Socket(){
        close();
    }

    void Socket::init(){
        static int is_init = 0;
		static int erreur = 0;
        if (is_init == 0){
            #if defined (WIN32)
                WSADATA WSAData;
                erreur = WSAStartup(MAKEWORD(2,2), &WSAData);
            #else
                erreur = 0;
            #endif
        }
        is_init++;
    }

    void Socket::fini(){
        static int is_init;
		static int erreur;
		is_init--;
        if (is_init == 0){
            #if defined (WIN32)
                WSACleanup();
            #endif
        }
    }


    void Socket::setAddress(std::string add){
        address = add;
    }

    std::string Socket::getAddress(){
        return address;
    }

    void Socket::setPort(short p){
        port = p;
    }

    short Socket::getPort(){
        return port;
    }

    int Socket::connect(){ //TODO : Include Logger functionnality somewhere.
        sockaddr_in server;
        server.sin_family       = AF_INET;
        server.sin_addr.s_addr  = inet_addr(address.c_str());
        server.sin_port         = htons(port);
        memset(&server.sin_zero, '\0', sizeof(server.sin_zero));
        if (::connect(sock, (sockaddr *)&server, sizeof(server)) == SOCKET_ERROR){
            std::cerr << "Failed connecting to " << address << ":"<<port<<std::endl; //Replace by error log.
            return -1;
        }
        return 0;
    }

    void Socket::close(){ //TODO Implement the tests.
    //Is Socket connected? If so,
        ::shutdown(sock,SHUT_RDWR);
    //Is Socket close? If so,
        ::close(sock);
    }

    int Socket::write(std::string message){
        send(sock, message.c_str(), message.length() + 1, 0); //The c_str contains one more character than the length of the string : \0
    }

    std::string Socket::recv(){
        std::string mess ="";
        char data[BUF_SIZE];
        ::recv(sock, data, BUF_SIZE, 0); //TODO Manage possible error raised by this function.
        mess = data;
        return mess;
    }
}

