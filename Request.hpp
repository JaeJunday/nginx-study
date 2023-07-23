#pragma once

#include <iostream>
#include <string>
#include "sys/socket.h"
#include <netinet/in.h> // sockaddr_in

class Request
{
    private:
        int         _socket;
        char*       _buffer;
        std::string _method;
        std::string _host;
        std::string _contentType;
        std::string _contentLength;
    public:
        Request(int socket);
        // void Request::parsing();
        void setSocket(int fd);
        int getSocket() const;
        const char* getBuffer() const;
        void setBuffer(char* buffer);
};
