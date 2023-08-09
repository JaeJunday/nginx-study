#pragma once

#include "Color.hpp"
#include "enum.hpp"
#include "Util.hpp"
#include "Server.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
//#include <queue>
#include <fcntl.h> 
//#include <deque>
/*
struct Buffer{
	std::string	_saved;
	int		_len;
};
*/

class Request
{
	private:
		const Server&		_server;
		Location*			_location;
		int					_state;
		int					_socket;
		std::string			_headerBuffer;
		std::string			_buffer;
		std::string			_method;
		std::string			_requestUrl;
		std::string			_version;
		std::string			_ip;
		uint32_t			_port;
		std::string			_connection;
		std::string			_contentType;
		unsigned int		_contentLength;
		std::string			_transferEncoding;
		std::string			_boundary;
		int					_eventState;
		std::string			_chunkedFilename;
		//std::deque<Buffer>	_chunkedBuffer;
		int 				_bodyIndex;
	public:
		Request(int socket, const Server& server);
		void headerParsing(char* buf, intptr_t size);
		void checkMultipleSpaces(const std::string& str);
		void clearRequest();
		// chunked
		//bool checkDeque(Request* req, int& lenToSave, std::string& updatedBuffer);
		//void endChunkedParsing(Request* req);
		//bool parseChunkedData(Request* req, const std::string& updatedBuffer);
		// get
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
		const std::string& getBoundary() const;
		Location* getLocation() const;
		const std::string& getContentType();
		int getEventState() const;
		const std::string& getBuffer() const;
		
		const std::string& getChunkedFilename();
		//std::deque<struct Buffer>& getChunkedBuffer();
		const int getBodyIndex() const;
		// set
		void setRequestLine(std::string& requestLine);
		void setFieldLine(std::string& fieldLine);
		void setBuffer(char *buffer, int size);
		void setLocation(Location* location);
		void setEventState(int eventState);
};
