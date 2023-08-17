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
	private:
		Server&				_server;
		Location*			_location;
		std::string			_headerBuffer;
		std::string			_requestBuffer;
		std::string			_method;
		std::string			_requestPath;
		std::string			_version;
		std::string			_ip;
		std::string			_connection;
		std::string			_contentType;
		std::string			_contentLength;
		std::string			_transferEncoding;
		std::string			_boundary;
		std::string			_chunkedFilename;
		std::string 		_perfectBody;
		std::string			_secretHeader;

		uint32_t			_port;
		int					_state;
		int					_eventState;
		int 				_bodyStartIndex;
		int 				_bodyTotalSize;
		// chunked
		int					_chunkedState;
		int					_readIndex;
		bool				_writeEventFlag;

	public:
		Request(Server& server);
		Request(const Request& request);
		Request& operator=(Request const& rhs);
		~Request();
		void 				headerParsing(char* buf, intptr_t size);
		void				checkMultipleSpaces(const std::string& str);
		void				clearRequest();
		void				makeResponse(int kq);
		// post, chunked
		void 				parseChunkedData(Client* client);
		void				initCgi();
		void				childProcess();

		// get
		int					getState() const;
		const Server& 		getServer() const;
		const std::string&	getIp() const;
		const std::string&	getMethod() const;
		const std::string&	getVersion() const;
		const std::string&	getRequestUrl() const;
		const std::string&	getTransferEncoding() const;
		const std::string&	getConnection() const;
		const std::string&	getContentLength() const;
		const std::string&	getBoundary() const;
		Location*			getLocation() const;
		const std::string&	getContentType();
		int					getEventState() const;
		const std::string& 	getBuffer() const;
		const std::string& 	getChunkedFilename();
		int 				getBodyIndex() const;
		int 				getChunkedState() const;
		int 				getBodyTotalSize() const;
		std::string&		getPerfectBody();
		int 				getBodyStartIndex() const;
		const std::string&	getSecretHeader() const;

		// set
		void setState(int state);
		void setRequestLine(std::string& requestLine);
		void setFieldLine(std::string& fieldLine);
		void setBuffer(char *buffer, int size);
		void setLocation(Location* location);
		void setEventState(int eventState);
		void setChunkedFilename(std::string& chunkedFilename);
		void setBodyTotalSize(int bodyTotalSize);
		void setPerfectBody(std::string& body);

};
//event process
// delete
//	add
// state set
