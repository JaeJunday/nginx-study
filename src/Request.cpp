#include "Request.hpp"

Request::Request(int socket, const Server& server)
    : _server(server), _state(0), _socket(socket), _port(0), _contentLength(0), _eventState(0)
{
}

void Request::checkMultipleSpaces(const std::string& str)
{
    char prev = '\0';
    for (size_t i = 0; i < str.size(); ++i) {
        if ((prev == ' ' || prev == '\n') && prev == str[i])
            throw std::runtime_error("Error: Invalid Octet Space or newLine");
        prev = str[i];
    }
}

void Request::setRequestLine(std::string& requestLine)
{
    checkMultipleSpaces(requestLine);
    std::vector<std::string> token = util::getToken(requestLine, " ");

    std::cerr << "========================requestLine========================" << std::endl;
    std::cerr << requestLine << std::endl;

    if (token.size() != 3)
    {
        std::cerr << "Error: Request Line size error" << std::endl;
        throw 400;
    }
    if (!(token[0] == "GET" || token[0] == "HEAD" || token[0] == "DELETE" || token[0] == "POST"))
        throw 400;
    if (token[2] != "HTTP/1.1")
        throw 400;
    _method = token[0];
    _requestUrl = token[1];
    _version = token[2];
}

void Request::setFieldLine(std::string& fieldLine)
{
    checkMultipleSpaces(fieldLine);
    std::vector<std::string> token = util::getToken(fieldLine, ": ");
    token[1].erase(0, 1);
    if (token.size() != 2)
        throw 401;
        //throw std::runtime_error("Error: Header error");
    if (token[0].empty() || token[1].empty())
        throw 402;
        // throw std::runtime_error("Error: Header Key Value Empty");
    if (token[0].find(' ') != std::string::npos)
        throw 403;
        // throw std::runtime_error("Error: Header Key have space");
    if (token[0] == "Host") {
        size_t mid = token[1].find(":");
        if (mid == std::string::npos)
        {
            std:: cerr << "ERROR : HOST ERROR" << std::endl;
            throw 404;
        }
        _ip = std::string(token[1], 0, mid);
        _port = util::stoui(std::string(token[1], mid + 1, token[1].size() - (mid + 1)));
    }
    if (token[0] == "Content-Type")
    {
        const std::string hash = "boundary=";
        if (int index = token[1].find(hash) != std::string::npos)
            _boundary = "--" + std::string(token[1], index + hash.length(), token[1].size() - index);
        else 
            _contentType = token[1]; 
    } 
    if (token[0] == "Content-Length")
        _contentLength = util::stoui(token[1]);
    if (token[0] == "Transfer-Encoding")
        _transferEncoding = token[1];
    if (token[0] == "Connection")
        _connection = token[1];
}

void Request::parsing(char* buf, intptr_t size)
{
    std::cerr << "========================parsing========================" << std::endl;
    // 헤더 끝줄 찾기
    std::string buffer(buf, size);
    _headerBuffer += buffer;
    int headerBoundary = _headerBuffer.find("\r\n\r\n");
    if (headerBoundary == std::string::npos)
        return ;
    _state = request::DONE;
    // 첫번째 라인일때 - Refactoring 나중에 함수로 뺀다 - kyeonkim
    // 한줄 밖에 없을 때 find 조건이 -1이 나와버림. 한줄도 넣어서 검사해야함 - kyeonkim
    int endLine = _headerBuffer.find("\r\n");
    std::string requestLine(_headerBuffer, 0, endLine);
    setRequestLine(requestLine);

    // 둘째줄부터 끝줄까지
    //int secondLine = buffer.find("\r\n", endLine + 2);
    int newEndLine;
    endLine += 2;
    while (endLine < headerBoundary) { 
        newEndLine = _headerBuffer.find("\r\n", endLine);
        std::string fieldLine(_headerBuffer, endLine, newEndLine - endLine);

        setFieldLine(fieldLine);
        endLine = newEndLine + 2;
    }
    int start = headerBoundary + 4;
    if (_headerBuffer.size() - start != 0)
        _buffer = _headerBuffer.substr(start, _headerBuffer.size() - start);
    if (_method == "POST")
        _state = request::POST;
}

std::string removeSpecificCharacter(std::string str, char ch)
{
	size_t pos = str.find(ch);

	while (pos != std::string::npos) {
		str.erase(pos, 1);
		pos = str.find(ch, pos);
	}
	return (str);
}

void Request::setBuffer(char *buffer, int size)
{
	_buffer += std::string(buffer, size);   
}

void Request::setEventState(int eventState)
{
    _eventState = eventState;
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

const Server& Request::getServer() const
{
    return _server;
}


Location* Request::getLocation() const
{
    return _location;
}
     
void Request::setLocation(Location* location)
{
    _location = location;
}

const std::string& Request::getBoundary() const
{
	return _boundary;
}

int Request::getEventState() const
{
    return _eventState;
}

const std::string& Request::getContentType()
{
    return _contentType;
}


const std::string& Request::getChunkedFilename()
{
    return _chunkedFilename;
}

bool Request::checkDeque(Request* req, size_t& lenToSave, std::string& updatedBuffer)
{
	bool head = false;

	if (_chunkedBuffer.empty() == false && \
		_chunkedBuffer.back()._len != _chunkedBuffer.back()._saved.length())
	{
		Buffer prevBuffer = _chunkedBuffer.back();
		_chunkedBuffer.pop_back();
	    lenToSave = prevBuffer._len;
		if (lenToSave != 0)
			head = true;
		updatedBuffer = prevBuffer._saved;
	}
	return head;
}

void Request::endChunkedParsing(Request* req)
{
	std::deque<struct Buffer> chunked = _chunkedBuffer;
	size_t totalLen = 0;
	std::string mergedBuffer;
    std::vector<std::string> url = util::getToken(_requestUrl, "/");

	// \r\n\r\n 만 남았는지 check하는 로직 추가하고, 
	// 그 외의 데이터가 들어 왔으면 다시 버퍼에 붙여서 parsing에 들어 가게 한다.

	// chunked send함수를 따로 빼준다. 불필요한 stringcopy를 줄이기 위해 -semikim

    if (url.size() >= 1)
        _chunkedFilename = url[url.size() - 1];

	while (chunked.size())
	{
		struct Buffer buf = chunked.front();
		totalLen += buf._len;
		mergedBuffer += buf._saved;
		chunked.pop_front();
	}
    // ---------------------------------------- testcode
    std::cerr << "totalLen:" << totalLen << std::endl;
    std::cerr << " length:" <<  _contentLength << std::endl;
    //std::cerr << "mergedBuffer\n" << mergedBuffer << std::endl;
    // ---------------------------------------- testcode
	
	// -totalLen 이 0일 때 오류 처리 해야함 - semikim
	
	// req->changeBuffer(mergedBuffer);
	// req->setContentLength(totalLen);
    _buffer = mergedBuffer;
    _contentLength = totalLen;
    // std::cerr << "=============chunked data====================" << std::endl;
    // std::cerr << _chunkedFilename << std::endl;
    // std::cerr << _contentType << std::endl;
    // std::cerr << _buffer << std::endl;
}


bool Request::parseChunkedData(Request* req, const std::string& updatedBuffer)
{
    size_t start = 0;
    std::string str;
    size_t i = 0;
    size_t e = 0;
    bool head = false;
    size_t lenToSave = 0;

    std::string tmp;

    head = checkDeque(req, lenToSave, tmp);
    std::string buffer = tmp + updatedBuffer;
    while (1)
    {
        if (head == false)
        {
			i = buffer.find("\r\n", start);
            if (i != std::string::npos)
            {
                char *end = NULL;
                str = buffer.substr(start, i - start);
                if (str.empty() == false)
                {
                    lenToSave = std::strtol(str.c_str(), &end, 16);
                	if (lenToSave == 0)
                	{
						endChunkedParsing(req);
                        // updatedBuffer 에 데이터가 남아 있다면 다음에 들어오는 요청 헤더일수도 있기 때문에 보관해야 한다.... ㅜㅜ -semikim
                        // \r\n\r\n 을 제거하고...... 어떻게 이어주지...............
                    	return (true);
                	}
				}
                head = true;
                i += 2;
            }
            else
            {
                Buffer buf;
                str = buffer.substr(start, std::string::npos);
                if (str.empty() == false)
				{
					buf._saved = str;
                	buf._len = 0;
                	_chunkedBuffer.push_back(buf);
				}
                break;
            }
        }
        if (head == true)
        {
			
			Buffer buf;
			buf._len = lenToSave;
            size_t st = i;

            while (1)
            {
                e = buffer.find("\r\n", i);
                if (e == std::string::npos)
                    break;
                if (e - st == lenToSave)
                    break;
                else
                    i = e + 2;
            }

            if (e != std::string::npos)
            {
                str = buffer.substr(st, e - st); 
                if (str.empty() == false)
                {
                    if (str.size() == lenToSave)
                    {
                        buf._saved = str;
                		_chunkedBuffer.push_back(buf);
                    }
                    else // ------------------------------ testcode  debug
                    {
                        std::cerr << "len false" << std::endl;
                    }
                    // ------------------------------------ testcode debug
                }
                head = false;
                if (e != std::string::npos)
                    e += 2;
                else
                    break;
            }
            else
            {
                str = buffer.substr(st, std::string::npos); 
                if (str.empty() == false)
                {
                    buf._saved = str;
                    _chunkedBuffer.push_back(buf);
                }
                break;
            }
        }
        start = e;
    }
    return (false);
}


std::deque<struct Buffer>& Request::getChunkedBuffer()
{
	return _chunkedBuffer;
}
