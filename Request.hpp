#pragma once

#include "enum.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fcntl.h> 

class Request
{
    private:
        int          _state;
        int          _socket;
        std::string  _main;
        std::string  _method;
		std::string  _requestUrl;
		std::string  _version;
        std::string  _ip;
        unsigned int _port;
        std::string  _contentType;
        unsigned int _contentLength;
    public:
        Request(int socket);
        void parsing(char* buf, intptr_t size);
        int getSocket() const;
        int getState() const;
        void setRequestLine(std::string requestLine);
        std::vector<std::string> getToken(std::string& str, const std::string& delimiters) const;
        void checkMultipleSpaces(const std::string& str);
        void setFieldLind(std::string fieldLine);
        void setMain(char *buffer, int size);
        const std::string& getMain() const;
};

// 이거 오퍼레이션에서 썼었음 리팩토링 ㄱ
unsigned int stoui(const std::string& str);
