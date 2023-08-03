#pragma once

#include "enum.hpp"
#include "Util.hpp"
#include "Server.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <fcntl.h> 

struct PostData{ std::string _data; std::string _filename; std::string _contentType; };

class Request
{
    private:
        const Server&			_server;
        Location*	 	_location;

        int          	_state;
        int          	_socket;
		std::string		_headerBuffer;
        std::string  	_buffer;
        std::string  	_method;
		std::string  	_requestUrl;
		std::string  	_version;
        std::string     _ip;
        uint32_t        _port;
        std::string     _connection;
        std::string     _contentType;
        unsigned int    _contentLength;
        std::string     _transferEncoding;
        std::vector<PostData> _files;
        std::string     _boundary;
    public:
        Request(int socket, const Server& server);
        void parsing(char* buf, intptr_t size);
        void bufferParsing();
        int getSocket() const;
        int getState() const;
        const Server& getServer() const;

        const std::string& getIp() const;
        const std::string& getMethod() const;
        const std::string& getVersion() const;
        const std::string& getRequestUrl() const;
        const std::string& getTransferEncoding() const;
        const std::string& getConnection() const;
        unsigned int getContentLength() const;

        void setRequestLine(std::string& requestLine);
        void checkMultipleSpaces(const std::string& str);
        void setFieldLind(std::string& fieldLine);
        void setBuffer(char *buffer, int size);
        const std::string& getBuffer() const;
        Location* getLocation() const;
        void setLocation(Location* location);
};
