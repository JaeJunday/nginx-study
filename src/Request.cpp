#include "Request.hpp"
#include "Client.hpp"

Request::Request(Server& server)
	: _server(server), 
	_location(NULL),
	_state(request::READY),
	_port(0), 
	_contentLength(0), 
	_eventState(0), 
	_bodyStartIndex(0), 
	_bodyTotalSize(-1),
	_chunkedState(chunk::READY),
	_readIndex(0),
	_writeEventFlag(false)
{
}

Request::Request(const Request& request)
{
	*this = request;
}

Request& Request::operator=(const Request& rhs)
{
	if (this != &rhs)
	{
		_server = rhs._server;
		_location = rhs._location;
		_state = rhs._state;
		_headerBuffer = rhs._headerBuffer;
		_requestBuffer = rhs._requestBuffer;
		_method = rhs._method;
		_requestUrl = rhs._requestUrl;
		_version = rhs._version;
		_ip = rhs._ip;
		_port = rhs._port;
		_connection = rhs._connection;
		_contentType = rhs._contentType;
		_contentLength = rhs._contentLength;
		_transferEncoding = rhs._transferEncoding;
		_boundary = rhs._boundary;
		_eventState = rhs._eventState;
		_bodyStartIndex = rhs._bodyStartIndex;	
		_bodyTotalSize = rhs._bodyTotalSize;
		_chunkedFilename = rhs._chunkedFilename;
		_chunkedState = rhs._chunkedState;
		_perfectBody = rhs._perfectBody;
		_readIndex = rhs._readIndex;
		_writeEventFlag = rhs._writeEventFlag;
	}
	return *this;	
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
	_bodyStartIndex = headerBoundary + 4;
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

void Request::parseChunkedData(Client* client)
{
	if (_readIndex == false)
		_readIndex = _bodyStartIndex;
	size_t index = _requestBuffer.find("\r\n", _readIndex);
	if (index != std::string::npos)
	{
		char*   endptr;
		size_t bodyStart = index + 2;
		size_t bodySize = std::strtol(_requestBuffer.c_str() + _readIndex, &endptr, HEX);
		if (endptr - _requestBuffer.c_str() != index)
			throw 400;
		if (bodySize == 0)
		{
			if (_requestBuffer.find("\r\n", bodyStart) != std::string::npos)
			{
				_bodyTotalSize = _requestBuffer.size() - _bodyStartIndex;
				// _response->endResponse();
				_chunkedState = chunk::END;
				return;
			}
		} 
		else if (bodyStart + bodySize + 2 <= _requestBuffer.length())//body뒤의 \r\n고려
		{	
			// _perpectBody add && pipe write event add
			if (_requestBuffer.find("\r\n", bodyStart + bodySize) != bodyStart + bodySize)
				throw 400;
			_perfectBody.append(_requestBuffer.substr(bodyStart, bodySize), bodySize);
			// dynamic_cast<Chunked *>(_response)->uploadFile(perfectBody); //cgi
			// _writeFd[1] event set required
			// client->addEvent(writeFd[1], client->getKq(), EVFILT_WRITE);
			//client->setWriteEvent();
			if (_writeEventFlag == false)
			{
				client->addEvent(client->getWriteFd(), EVFILT_WRITE);
				_writeEventFlag = true;
			}
			_readIndex = bodyStart + bodySize + 2;//body뒤의 \r\n고려
			_chunkedState =  chunk::CONTINUE;
			return;
		}
	}
	_chunkedState = chunk::INCOMPLETE_DATA;
}

// void Request::makeResponse(int kq)
// {
// 	if (_method.empty())
// 		throw 405;
// 	if (_method == "GET")
// 		_response = new Get(this, kq);
// 	else if (_transferEncoding == "chunked")
// 	{
// 		_response = new Chunked(this, kq);
// 		_response->createResponse();
// 	}
// 	else if (_method == "POST" || _method == "PUT")
// 		_response = new Post(this, kq);
// 	else if (_method == "DELETE")
// 		_response = new Delete(this, kq);
// 	_state = request::DONE;
// }

void Request::clearRequest()
{
	_location = NULL;
	// _response = NULL;
	_state = 0;
	_headerBuffer = "";
	_requestBuffer = "";
	_method = "";
	_requestUrl = "";
	_version = "";
	_connection = "";
	_contentType = "";
	_contentLength = 0;
	_transferEncoding = "";
	_boundary = "";
	_bodyStartIndex = 0;
	_readIndex = 0;
	// _writeIndex = 0;
	_chunkedState = 0;
	_writeEventFlag = false;
}

void Request::setBuffer(char *buffer, int size)
{
	_requestBuffer.append(buffer, size);
}

void Request::setEventState(int eventState)
{
	_eventState = eventState;
}

void Request::setState(int state)
{
	_state = state;
}

void Request::setPerfectBody(std::string perfectBody)
{
	_perfectBody = perfectBody;
}

void Request::setBodyTotalSize(int bodyTotalSize)
{
	_bodyTotalSize = bodyTotalSize;
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

const std::string& Request::getBuffer() const
{
	return _requestBuffer;
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
	return _bodyStartIndex;
}

// Client* Request::getResponse() const
// {
// 	return _response;
// }

int Request::getChunkedState() const
{
	return _chunkedState;
}

int Request::getBodyTotalSize() const
{
	return _bodyTotalSize;
}


std::string& Request::getPerfectBody()
{
	return _perfectBody;
}

void Request::setChunkedFilename(std::string& chunkedFilename)
{
	_chunkedFilename = chunkedFilename;
}

int Request::getBodyStartIndex() const
{
	return _bodyStartIndex;
}

