#include "Request.hpp"

Request::Request(int socket)
    : _state(0), _socket(socket), _port(0), _contentLength(0)
{
}

// GET / HTTP/1.1
// Host: 0.0.0.0:4242
// Connection: keep-alive
// Upgrade-Insecure-Requests1
// User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7
// Accept-Encoding: gzip, deflate
// Accept-Language: en-US,en;q=0.9,ko;q=0.8
// Transfer-encoding: chunked
// main

void Request::checkMultipleSpaces(const std::string& str)
{
    char prev = '\0';
    for (int i = 0; i < str.size(); ++i) {
        if ((prev == ' ' || prev == '\n') && prev == str[i])
            throw std::runtime_error("Error: Invalid Octet Space or newLine");
        prev = str[i];
    }
}

void Request::setRequestLine(std::string& requestLine)
{
    checkMultipleSpaces(requestLine);
    std::vector<std::string> token = util::getToken(requestLine, " ");

    if (token.size() != 3)
        throw std::runtime_error("Error: Request Line size error");
        
    if (!(token[0] == "GET" || token[0] == "HEAD" || token[0] == "DELETE" || token[0] == "POST"))
        throw std::runtime_error("Error: method error");
    if (token[2] != "HTTP/1.1")
        throw std::runtime_error("Error: version error");
    _method = token[0];
    _requestUrl = token[1];
    _version = token[2];
}

void Request::setFieldLind(std::string& fieldLine)
{
    checkMultipleSpaces(fieldLine);
    std::vector<std::string> token = util::getToken(fieldLine, ": ");
    token[1].erase(0, 1);
    if (token.size() != 2)
        throw std::runtime_error("Error: Header error");
    if (token[0].empty() || token[1].empty())
        throw std::runtime_error("Error: Header Key Value Empty");
    if (token[0].find(' ') != std::string::npos)
        throw std::runtime_error("Error: Header Key have space");
    if (token[0] == "Host") {
        int mid = token[1].find(":");
        if (mid == std::string::npos)
            throw std::runtime_error("Error: Host Error");
        _ip = std::string(token[1], 0, mid);
        _port = util::stoui(std::string(token[1], mid + 1, token[1].size() - (mid + 1)));
    }
    if (token[0] == "Content-Type")
    {
        const std::string hash = "boundary=";
        if (int index = token[1].find(hash))
            _boundary = std::string(token[1], index + hash.length(), token[1].size() - index);
        else 
            _contentType = token[1]; 
    } 
    if (token[0] == "Content-Length")
        _contentLength = util::stoui(token[1]);
    if (token[0] == "Transfer-encoding")
        _transferEncoding = token[1];
    if (token[0] == "Connection")
        _connection = token[1];
}

// http://0.0.0.0:4242
void Request::parsing(char* buf, intptr_t size)
{
    try 
	{
        // 헤더 끝줄 찾기
        std::string buffer(buf, size);
        int headerBoundary = buffer.find("\r\n\r\n");
        if (headerBoundary == -1)
            std::cout << "not found : send error page";

        // 첫번째 라인일때 - Refactoring 나중에 함수로 뺀다 - kyeonkim
        int endLine = buffer.find("\r\n");
        std::string requestLine(buffer, 0, endLine);
        setRequestLine(requestLine);

        // 둘째줄부터 끝줄까지
        //int secondLine = buffer.find("\r\n", endLine + 2);
        int newEndLine;
        endLine += 2;
        while (endLine < headerBoundary) { 
            newEndLine = buffer.find("\r\n", endLine);
            std::string fieldLine(buffer, endLine, newEndLine - endLine);

            setFieldLind(fieldLine);
            endLine = newEndLine + 2;
        }
        if (_method == "POST")
        {
            _state = request::POST;
            // if (_transferEncoding == "chunked")
            //     _state = 
        }
        std::cout << std::endl;
        std::cout << "우리가 넣은 값" << std::endl;
        std::cout << "method: " << _method << std::endl;
        std::cout << "_requestUrl: " <<_requestUrl << std::endl;
        std::cout << "_version: " << _version << std::endl;
        std::cout << "_ip: " << _ip << std::endl;
        std::cout << "port: " << _port << std::endl;
        std::cout << "c type: " << _contentType << std::endl;
        std::cout << "c len: " << _contentLength << std::endl;
        std::cout << "Transfer-encoding: " << _transferEncoding << std::endl;
        std::cout << "connection: " << _connection << std::endl;
        std::cout << "boundary: " << _boundary << std::endl;
        std::cout << std::endl;
        
// std::cout << fieldLine << std::endl;
        
            // 1. 개행 기준분리
            // 2. 연속 공백과 연속개행 체크 
            // 3. 1줄에서 첫번째 ": " 기준으로 앞은 키, 뒤는 밸류
//##################################################################################
            // 4. 키에서 필요한 값만 찾아서 세팅, 키안에 공백있으면 에러, 밸류가 비어있으면 에러
        // }

            // std::cout << line << std::endl;
            // 두번째 라인부터 \r\n \r\n 이 나올 때까지 파싱
            // 만약 \n이 없는 \r이 나온다면 무시하기
            // 만약 \r\n 이 아닌 \n 만 나온다면 개행과 동일하게 처리
			// checkOtherLine(line);
        // } 
	} catch (std::runtime_error &e) { 
        std::cerr << e.what() << std::endl;
    } catch (std::exception &e) { 
        std::cerr << e.what() << std::endl;
    }
}

int Request::getState() const
{
    return _state;
}


int Request::getSocket() const
{
    return _socket;
}

const std::string& Request::getBuffer() const
{
    return _buffer;
}

void Request::bufferParsing()
{
    // std::stringstream bufferStream;
    // bufferStream << _buffer;

    // std::string temp;
    // int state = file::START;

    // if (getline(bufferStream, temp))
    //     if (temp != _boundary)
    //         return;

    // while (getline(bufferStream, temp))
    // {
    //     if (temp == _boundary && state == file::START)
    //         state = file::HASH;
    //     else if(state == file::HASH)
    //     {
    //         size_t i = temp.find("filename=\"");
    //         if (i == std::string::npos)
    //     }

    // }
        
}

//         HASH = 0,
//         HEADER = 1,
//         CONTENT = 2,
//         END = 3

void Request::setBuffer(char *buffer, int size)
{
    _buffer += std::string(buffer, size);
}

const std::string& Request::getBufferTunnel() const
{
    return _bufferTunnel;
}

void Request::setBufferTunnel(char *buffer, int size)
{
    _bufferTunnel += std::string(buffer, size);   
}

const std::string& Request::getConnection() const
{
    return _connection;
}

const std::string& Request::getTransferEncoding() const
{
    return _transferEncoding;
}

unsigned int Request::getContentLength() const
{
    return _contentLength;
}




const std::string& Request::getIp() const
{
    return _ip;
}

const std::string& Request::getMethod() const
{
    return _method;
}

const std::string& Request::getVersion() const
{
	return _version;
}

const std::string& Request::getRequestUrl() const
{
    return _requestUrl;
}