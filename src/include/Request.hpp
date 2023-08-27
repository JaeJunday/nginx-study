#pragma once

// #include "Client.hpp"
#include "Color.hpp"
#include "enum.hpp"
#include "Util.hpp"
#include "Server.hpp"
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>
#include <fcntl.h> 
#include <cstdint>

#define HEX 16

class Client;

class Request
{
	// 현재 변수명이랑 함수명이랑 안맞는게 너무 많음 나중에 리네임 필요 - kyeonkim
	private:
		std::vector<Server>	_servers;
		Server*				_server;
		Location			_location;
		std::string			_headerBuffer;
		std::string			_requestBuffer;
		std::string			_method;
		std::string			_requestPath;
		std::string			_version;
		std::string			_host;
		std::string			_connection;
		std::string			_contentType;
		std::string			_contentLength;
		std::string			_transferEncoding;
		std::string			_boundary;
		std::string			_chunkedFilename;
		std::string 		_requestBody;
		std::string			_secretHeader;
		std::string			_convertRequestPath;

		uint32_t			_port;
		int					_state;
		// int					_eventState;
		int 				_bodyStartIndex;
		int 				_bodyTotalSize;
		// chunked
		bool				_chunkedEnd;
		int					_readIndex;

	public:
		Request(Server* server);
		Request(std::vector<Server>& servers);
		Request(const Request& request);
		Request& operator=(Request const& rhs);
		~Request();
		void headerParsing(int fd);
		void				checkMultipleSpaces(const std::string& str);
		void				clearRequest();
		void				makeResponse(int kq);
		// post, chunked
		void 				parseChunkedData(Client* client);
		void				handlePostCgi();
		void				childProcess();

		// get
		int					getState() const;
		const Server* 		getServer() const;
		const std::string&	getHost() const;
		const std::string&	getMethod() const;
		const std::string&	getVersion() const;
		// const std::string&	getRequestUrl() const;
		const std::string&	getTransferEncoding() const;
		const std::string&	getConnection() const;
		const std::string&	getContentLength() const;
		const std::string&	getBoundary() const;
		const Location*		getLocation();
		const std::string&	getContentType();
		// int					getEventState() const;
		const std::string& 	getBuffer() const;
		const std::string& 	getChunkedFilename();
		int 				getBodyIndex() const;
		int 				getChunkedEnd() const;
		int 				getBodyTotalSize() const;
		std::string&		getRequestBody();
		int 				getBodyStartIndex() const;
		const std::string&	getSecretHeader() const;
		const std::string&	getConvertRequestPath() const;
		Server*				findServer();

		std::string findLocationPath();
		void checkLimitExcept() const;
		void handleRequest(const struct kevent& tevent, char* buffer);

		// set
		void setState(int state);
		void setRequestLine(std::string& requestLine);
		void setFieldLine(std::string& fieldLine);
		void setBuffer(char *buffer, int size);
		// void setLocation(Location* location);
		// void setEventState(int eventState);
		void setChunkedFilename(std::string& chunkedFilename);
		void setBodyTotalSize(int bodyTotalSize);
		void setRequestBody(std::string& body);
		void setChunkedEnd(bool set);
		void setServer(Server* server);
};
