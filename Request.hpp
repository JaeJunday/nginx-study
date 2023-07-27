#pragma once

#include "enum.hpp"
#include "Util.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <fcntl.h> 

struct PostData{ std::string _boundary; std::string _filename; std::string _contentType; };

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
        uint32_t     _port;
        std::string  _connection;
        std::string  _contentType;
        unsigned int _contentLength;
        std::string  _transferEncoding;
        std::vector<PostData> _files;
        std::queue<std::string> _bufferQueue;
    public:
        Request(int socket);
        void parsing(char* buf, intptr_t size);
        int getSocket() const;
        int getState() const;

        const std::string& getIp() const;
        const std::string& getMethod() const;
        const std::string& getVersion() const;
        const std::string& getRequestUrl() const;
        const std::string& getTransferEncoding() const;
        const std::string& getConnection() const;

        void setRequestLine(std::string& requestLine);
        void checkMultipleSpaces(const std::string& str);
        void setFieldLind(std::string& fieldLine);
        void setMain(char *buffer, int size);
        const std::string& getMain() const;
};
