#pragma once

#include <iostream> 
#include <string> 
#include <ctime>
#include <sstream>
#include <fstream>
#include <vector>
#include <dirent.h>

#define TIME_SIZE 40

class Request;
class Server;

class AResponse
{
	protected:
		std::string			_version;
		std::string			_stateCode;
		std::string			_reasonPhrase;
		std::time_t			_date;
		std::string 		_serverName;
		std::string			_contentType;
		size_t				_contentLength;
		std::stringstream	_buffer;

		// 삭제한 request 를 가리킬 가능성이 있는가??
		Request*		_request;
		
		AResponse(const AResponse& src); 
		AResponse& operator=(AResponse const& rhs);
	public:
		AResponse();
		virtual ~AResponse();
		AResponse(Request* request);
		std::string getDate() const;
		virtual void createResponse() =0;
		void stamp() const;
		const std::stringstream& getBuffer() const;
		std::string findLocationPath() const;
};
