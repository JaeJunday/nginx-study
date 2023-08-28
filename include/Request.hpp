#pragma once

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

class Client;

class Request
{
	private:
		std::vector<Server>	_servers;
		Server*				_server; 			 // set, get
		Location			_location; 			 // get
		std::string			_headerBuffer;
		std::string			_requestBuffer; 	 // get
		std::string			_method; 			 // set, get 
		std::string			_requestPath;		 // set
		std::string			_version; 			 // set, get
		std::string			_host; 				 // set, get
		std::string			_connection; 		 // set, get
		std::string			_contentType; 		 // set, get
		std::string			_contentLength; 	 // set, get
		std::string			_transferEncoding;	 // set, get
		std::string			_boundary; 			 // set, get
		std::string 		_requestBody; 		 // set, get
		std::string			_secretHeader; 		 // set, get
		std::string			_convertRequestPath; // get
		uint32_t			_port;				 // set
		int					_state;
		int 				_bodyStartIndex; 	 // get
		int 				_bodyTotalSize; 	 // set, get
		bool				_chunkedEnd; 		 // set, get
		int					_readIndex;

	public:
		// Request.cpp
		Request(std::vector<Server>& servers);
		Request(const Request& request);
		Request& operator=(Request const& rhs);
		~Request();
		void 				handleRequest(const struct kevent& tevent, char* buffer);
		void 					parsingHeader();
		Server*					findServer();
		std::string 			findLocationPath();
		void 					checkLimitExcept() const;
		void				clearRequest();
		// RequestChunked.cpp
		void 				parsingChunkedData();
		bool 					parseChunkedBody(char* requestBuffer, size_t requestBufferSize);
		void 						handleChunkedEnd(size_t bodyStart);
		void 						appendChunkedData(char* requestBuffer, size_t bodyStart, size_t bodySize);
		// RequestGetter.cpp
		const Server* 		getServer() const;
		const std::string&	getHost() const;
		const std::string&	getMethod() const;
		const std::string&	getVersion() const;
		const std::string&	getTransferEncoding() const;
		const std::string&	getConnection() const;
		const std::string&	getContentLength() const;
		const std::string&	getBoundary() const;
		const Location*		getLocation();
		const std::string&	getContentType();
		const std::string& 	getRequestBuffer() const;
		int 				getBodyIndex() const;
		int 				getChunkedEnd() const;
		int 				getBodyTotalSize() const;
		std::string&		getRequestBody();
		int 				getBodyStartIndex() const;
		const std::string&	getSecretHeader() const;
		const std::string&	getConvertRequestPath() const;
		// RequestSetter.cpp
		void				setServer(Server* server);
		void				setRequestLine(std::string& requestLine);
		void				setFieldLine(std::string& fieldLine);
		void				setBodyTotalSize(int bodyTotalSize);
		void				setRequestBody(std::string& body);
		void				setChunkedEnd(bool set);
};
