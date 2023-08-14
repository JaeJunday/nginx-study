#pragma once

#include "Client.hpp"

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

#define HEX 16

class Request
{
	private:
		int					_writeFd[2]; // parent(w) -> child(r)
		int					_readFd[2]; // child(w) -> parent(r)
		const Server&		_server;
		Location*			_location;
		Client*				_response;
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
		int 				_bodyStartIndex;

		int 				_bodyTotalSize;
		// chunked
		int					_chunkedState;
		std::string 		_perfectBody;
		int					_readIndex;
		int					_writeIndex;
		
	public:
		Request(int socket, const Server& server);
		void				headerParsing(char* buf, intptr_t size);
		void				checkMultipleSpaces(const std::string& str);
		void				clearRequest();
		void				makeResponse(int kq);
		// post, chunked
		void				parseChunkedData();
		void				initCgi();
		void				childProcess();

		// get
		int					getSocket() const;
		int					getState() const;
		const Server& 		getServer() const;
		const std::string&	getIp() const;
		const std::string&	getMethod() const;
		const std::string&	getVersion() const;
		const std::string&	getRequestUrl() const;
		const std::string&	getTransferEncoding() const;
		const std::string&	getConnection() const;
		unsigned int 		getContentLength() const;
		const std::string&	getBoundary() const;
		Location*			getLocation() const;
		const std::string&	getContentType();
		int					getEventState() const;
		const std::string& 	getBuffer() const;
		const std::string& 	getChunkedFilename();
		int 				getBodyIndex() const;
		Client* 			getResponse() const;
		int 				getChunkedState() const;
		int 				getWriteFd() const;
		int 				getBodyTotalSize() const;

		// set
		void setRequestLine(std::string& requestLine);
		void setFieldLine(std::string& fieldLine);
		void setBuffer(char *buffer, int size);
		void setLocation(Location* location);
		void setEventState(int eventState);
};
