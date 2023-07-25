#include "Get.hpp"
#include "Request.hpp"

Get::Get() : AResponse()
{ 
}

Get::Get(const Get& src)
{
    /* Copy Constructor Implementation */
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
	_buffer << "Content-Length: " << _contentLength << "\r\n\r\n";
}

void Get::createResponseMain()
{
    std::ifstream	file;
	std::string		line;

    if (_request->getRequestUrl() == "/")
	{
		file.open("resource/hello.html", std::ios::binary);
		if (file.eof() == false)
            error;
        _buffer << file.rdbuf();
	}
}