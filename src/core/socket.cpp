/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * epyx
 * Copyright (C) aniem 2012 <jeremy.buet@polytechnique.org>
 * 
 */

#include "socket.h"

#define BUF_SIZE 4096

Socket::Socket(){
	outBuffer ="";
	inBuffer = "";
	address ="";
	port = -1;
	socket = socket(PF_INET,SOCK_STREAM,0);
	init();
}

Socket::Socket(String add, int p){
	Socket();
	address = add;
	port = p;
}

Socket::~Socket(){
	close();
}

static void Socket::init(){
	static int is_init = 0;
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

static void Socket::fini(){
	is_init--;
	if (is_init == 0){
		#if defined (WIN32)
	        WSACleanup();
	    #endif
	}
}


void Socket::setAddress(String add){
	address = add;
}

String Socket::getAddress(){
	return address;
}

void Socket::setPort(short p){
	port = p;
}

short Socket::getPort()
	return port;
}

int Socket::connect(){ //TODO : Include Logger functionnality somewhere.
	SOCKADDR_IN server;
	server.sin_family       = AF_INET;
    server.sin_addr.s_addr  = inet_addr(address);
    server.sin_port         = htons(port);
    memset(&server.sin_zero, '\0', sizeof(server.sin_zero));
	if (connect(s, (SOCKADDR *)&server, sizeof(server)) == SOCKET_ERROR){
		std::cerr << "Failed connecting to " << address << ":"<<port<<std::endl //Replace by error log.
		return -1;
	}
	return 0;
}

int Socket::close(){ //TODO Implement the tests.
//Is Socket connected? If so,
	shutdown(socket,SD_SEND);
//Is Socket close? If so, 
	close(socket);
}

int Socket::write(String message){
	send(socket, message.c_str(), message.length() + 1, 0); //The c_str contains one more character than the length of the string : \0 
}

String Socket::recv(){
	String mess ="";
	char[BUF_SIZE] data;
	recv(socket, data, BUF_SIZE, 0); //TODO Manage possible error raised by this function.
	mess = data;
	return mess;
}