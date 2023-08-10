#pragma once 

#include "AResponse.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/socket.h> // socket

class Error : public AResponse
{
public:
    Error(Request* request, int kq);
	void createResponse(); // override
	void makeErrorPage(int errnum);
	void pushErrorBuffer(std::string body, int errnum);
};
 