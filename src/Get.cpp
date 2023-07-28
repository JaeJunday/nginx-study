#include "Get.hpp"
#include "Request.hpp"

Get::Get() : AResponse()
{ 
}

Get::Get(Request* request) : AResponse()
{
    _request = request;
}

Get::~Get()
{
    /* Destructor Implementation */
}

Get& Get::operator=(Get const& rhs)
{
    if (this != &rhs)
    {
        /* Assignment Operator Implementation */
    }
    return *this;
}	

/*
// 형식
<버전> <상태 코드> <사유 구절>
<헤더>

<본문>

// ex
HTTP/1.1 200 OK
Content-type: text/html
Context-length: 42
...

Body line1
Body line2
...
HTTP/1.1 200 OK
Date: Tue, 20 Jul 2023 12:34:56 GMT
Server: Apache/2.4.38 (Unix)
Content-Type: text/html; charset=UTF-8
Content-Length: 1024
*/

void Get::createResponseHeader()
{
	_buffer << _version << " " << _stateCode << " " << _reasonPhrase << "\r\n";
	_buffer << "Date: " << getDate() << "\r\n";
	_buffer << "Server: " << _serverName << "\r\n";
	_buffer << "Content-Type: " << _contentType << "\r\n";
    std::ifstream	file;
    std::stringstream tmp;
    // 경로에 따라서 맞는 경로의 파일을 오픈
    if (_request->getRequestUrl() == "/")
	{

		file.open("src/pages/hello.html");
		if (file.is_open() == false)
            throw std::runtime_error("Error: file not found error");
        tmp << file.rdbuf();
        _contentLength = tmp.str().length();
        file.close();
	}
    else
    {
        file.open("src/pages/error/404.html");
		if (file.is_open() == false)
            throw std::runtime_error("Error: file not found error");
        tmp << file.rdbuf();
        _contentLength = tmp.str().length();
        file.close();
    }
	_buffer << "Content-Length: " << _contentLength << "\r\n\r\n";
}

void Get::createResponseMain()
{
    std::ifstream	file;

    if (_request->getRequestUrl() == "/")
	{
		file.open("src/pages/hello.html");
		if (file.is_open() == false)
            throw std::runtime_error("Error: file not found error");
        _buffer << file.rdbuf();
        file.close();
	}
    else
    {
		file.open("src/pages/error/404.html");
		if (file.is_open() == false)
            throw std::runtime_error("Error: file not found error2222");
        _buffer << file.rdbuf();
        file.close();
    }
}