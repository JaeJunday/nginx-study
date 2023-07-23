#pragma once
#include <sys/types.h>  // socket, bind
#include <sys/socket.h> // socket
#include <netinet/in.h> // sockaddr_in
#include <fcntl.h>      // fcntl

#define MAX_EVENTS 10

struct Socket
{
	int 			_fd;
	// sockaddr_in		_addr;
	// socklen_t		_len;
};
