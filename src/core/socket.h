/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * epyx
 * Copyright (C) aniem 2012 <jeremy.buet@polytechnique.org>
 * 
 */

#ifndef EPYX_SOCKET_H_
#define EPYX_SOCKET_H_

#include<sys/socket.h>
#include<stdio.h>
#include<String>

class Socket
{
public:
	int connect();
	Socket();
	static void init();
	static void fini();
	Socket(String adress, short port);
	void setAddress(String address);
	void setPort(short port);
	short getPort();
	String getAddress();
	int close();
	int write(String message);
	String recv();
	~Socket();
protected:

private:
	static int erreur;
	String Address;
	short port;

	int socket;
	
//	String outBuffer;
//	String inBuffer;
};

#endif // EPYX_SOCKET_H_
