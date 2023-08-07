#pragma once 

#include "AResponse.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/socket.h> // socket

struct ErrorData
{
	int			_stateCode;
	std::string	_reasonPhrase;
	std::time_t	_date;
	std::string _serverName;
	std::string	_contentType;
	size_t		_contentLength;
	std::string	_buffer;
};

void sendErrorPage(int fd, int errnum);
void pushErrorBuffer(ErrorData& data, std::string body);