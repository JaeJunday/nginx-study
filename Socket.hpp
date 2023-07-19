#pragma once
#include <sys/types.h>  // socket, bind
#include <sys/socket.h> // socket
#include <netinet/in.h> // sockaddr_in
#include <sys/event.h>  // kqueue

#define MAX_EVENTS 10

struct Socket
{
	int server_fd, client_fd;
	int kq, nev;
	sockaddr_in		server_addr, client_addr;
	socklen_t		client_len;
	struct kevent	event, events[MAX_EVENTS];
};