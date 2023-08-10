#include "Request.hpp"

Request::Request(int socket, const Server& server)
	: _server(server), 
	_response(NULL), 
	_location(NULL),
	_state(request::READY),
	_socket(socket), 
	_port(0), 
	_contentLength(0), 
	_eventState(0), 
	_bodyIndex(0), 
	_chunkedIndex(0), 
	_chunkedState(chunk::READY)
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
// testcode 
	int         bodySize;
	// std::cerr << "========================requestLine========================" << std::endl;
	// std::cerr << requestLine << std::endl;

	if (token.size() != 3)
	{
		std::cerr << "Error: Request Line size error" << std::endl;
		throw 400;
	}
	if (!(token[0] == "GET" || token[0] == "DELETE" || token[0] == "POST" || token[0] == "PUT"))
		throw 405;
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


// 헤더 + 본문 길이 = buffersize
// 본문길이  = contentLength
// 본문 스타트 길이 = bodyindex

// buffersize - bodyindx == contesntLength : ok
void Request::headerParsing(char* buf, intptr_t size)
{
	// testcode
	// std::cerr << "========================parsing========================" << std::endl;
	// 헤더 끝줄 찾기
	std::string buffer(buf, size);
	_headerBuffer += buffer;
	int headerBoundary = _headerBuffer.find("\r\n\r\n");
	if (headerBoundary == std::string::npos)
		return ;
	std::cout << RED << "testcode " << "====_headerBuffer\n" << _headerBuffer << RESET << std::endl;
	_state = request::CREATE;
	int endLine = _headerBuffer.find("\r\n");
	std::string requestLine(_headerBuffer, 0, endLine);
	setRequestLine(requestLine);

	int newEndLine;
	endLine += 2;
	while (endLine < headerBoundary) { 
		newEndLine = _headerBuffer.find("\r\n", endLine);
		std::string fieldLine(_headerBuffer, endLine, newEndLine - endLine);

		setFieldLine(fieldLine);
		endLine = newEndLine + 2;
	}
	_bodyIndex = headerBoundary + 4;
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

void Request::parseChunkedData()
{
	if (_chunkedIndex == false)
		_chunkedIndex = _bodyIndex;
	size_t index = _buffer.find("\r\n", _chunkedIndex);
	if (index != std::string::npos)
	{
		char*   endptr;
		size_t bodyStart = index + 2;
		size_t bodySize = std::strtol(_buffer.c_str() + _chunkedIndex, &endptr, HEX);
		if (endptr - _buffer.c_str() != index)
			throw 400;
		if (bodySize == 0)
		{
			if (_buffer.find("\r\n", bodyStart) != std::string::npos)
			{
				dynamic_cast<Chunked *>(_response)->endResponse();
				_chunkedState = chunk::END;
				return;
			}
		} 
		else if (bodyStart + bodySize + 2 <= _buffer.length())//body뒤의 \r\n고려
		{
			std::cerr << B_RED << "testcode " << "CGI 춰리" << RESET << std::endl;
			if (_buffer.find("\r\n", bodyStart + bodySize) != bodyStart + bodySize)
				throw 400;
			std::string perfectBody = _buffer.substr(bodyStart, bodySize);
			dynamic_cast<Chunked *>(_response)->uploadFile(perfectBody); //cgi
			_chunkedIndex = bodyStart + bodySize + 2;//body뒤의 \r\n고려
			_chunkedState =  chunk::CONTINUE;
			return;
		}
	}
	_chunkedState = chunk::INCOMPLETE_DATA;
}

void Request::makeResponse(int kq)
{
	if (_method.empty())
		throw 405;
	if (_method == "GET")
		_response = new Get(this, kq);
	else if (_transferEncoding == "chunked")
	{
		_response = new Chunked(this, kq);
		_response->createResponse();
	}
	else if (_method == "POST" || _method == "PUT")
		_response = new Post(this, kq);
	else if (_method == "DELETE")
		_response = new Delete(this, kq);
	_state = request::DONE;
}

void Request::clearRequest()
{
	_location = NULL;
	_response = NULL;
	_state = 0;
	_headerBuffer = "";
	_buffer = "";
	_method = "";
	_requestUrl = "";
	_version = "";
	_connection = "";
	_contentType = "";
	_contentLength = 0;
	_transferEncoding = "";
	_boundary = "";
	_bodyIndex = 0;
	_chunkedIndex = 0;
	_chunkedState = 0;
}

void Request::setBuffer(char *buffer, int size)
{
	_buffer.append(buffer, size);
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

int Request::getBodyIndex() const
{
	return _bodyIndex;
}

AResponse* Request::getResponse() const
{
	return _response;
}

int Request::getChunkedState() const
{
	return _chunkedState;
}