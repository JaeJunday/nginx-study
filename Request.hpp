#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <netinet/in.h> // sockaddr_in
#include <sys/types.h>  // socket, bind
#include <sys/socket.h> // socket
#include <fcntl.h> 

class Request
{
    private:
        int         _socket;
        std::string _method;
		std::string _requestUrl;
        std::string _host;
        std::string _contentType;
        std::string _contentLength;
    public:
        Request(int socket);
        void parsing(char* buffer);
        void setSocket(int fd);
        int getSocket() const;
        void checkFirstLine();
        void checkOtherLine();
};
