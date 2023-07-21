#pragma once
#include <sys/types.h>  // socket, bind
#include <sys/socket.h> // socket
#include <netinet/in.h> // sockaddr_in
#include <fcntl.h>      // fcntl

#define MAX_EVENTS 10

struct Socket
{
	int serverFd, clientFd;
	sockaddr_in		server_addr, client_addr;
	socklen_t		client_len;
};