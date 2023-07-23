#include "Request.hpp"

Request::Request(int socket)
    : _socket(socket)
{
}

// Request::~Request()
// {
// }

// void Request::checkHeader() const
// { 

// }

// GET / HTTP/1.1
// Host: 0.0.0.0:4242;
// Connection: keep-alive
// Upgrade-Insecure-Requests1
// User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7
// Accept-Encoding: gzip, deflate
// Accept-Language: en-US,en;q=0.9,ko;q=0.8

// method, requestUrl
// 1. line require (colon + space).
// 2. POST method required main.

void Request::checkFirstLine()
{

}

void Request::checkOtherLine()
{

}

void Request::parsing(char* buffer)
{
    try 
	{
        std::string line;
		std::string bufferString(buffer);
        std::istringstream buf(bufferString);
		
		getline(buf, line);
		checkFirstLine(line); // 첫 라인만 가져와서 파싱
        while (getline(buf, line))
        {
            // 두번째 라인부터 \r\n \r\n 이 나올 때까지 파싱
            // 만약 \n이 없는 \r이 나온다면 무시하기
            // 만약 \r\n 이 아닌 \n 만 나온다면 개행과 동일하게 처리
			checkOtherLine(line);
        }

	} catch (std::exception &e) { 
        std::cout << e.what() << std::endl;
    }
}

int Request::getSocket() const
{
    return _socket;
}