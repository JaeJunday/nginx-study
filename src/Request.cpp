#include "Request.hpp"
#include "Client.hpp"
#include <sys/errno.h>

Request::Request(Server* server)
	: _server(server),
	_location(NULL),
	_port(0), 
	_state(request::READY),
	_eventState(0), 
	_bodyStartIndex(0), 
	_bodyTotalSize(0),
	_chunkedEnd(false),
	_readIndex(0)
{
// std::cerr << CYAN << "testcode overloding constructor" << RESET << std::endl;
}

Request::Request(std::vector<Server>& servers)
	:_servers(servers),
	_location(NULL),
	_port(0), 
	_state(request::READY),
	_eventState(0), 
	_bodyStartIndex(0), 
	_bodyTotalSize(0),
	_chunkedEnd(false),
	_readIndex(0)
{
}


Request::Request(const Request& request)
	: _server(request._server)
{
// std::cerr << CYAN << "testcode copy constructor" << RESET << std::endl;
	*this = request;
}

Request::~Request()
{
}

Request& Request::operator=(const Request& rhs)
{
// std::cerr << BLUE << "assign operator" << RESET << std::endl;
	if (this != &rhs)
	{
		_servers = rhs._servers;
		_server = rhs._server;
		_location = rhs._location;
		_state = rhs._state;
		_headerBuffer = rhs._headerBuffer;
		_requestBuffer = rhs._requestBuffer;
		_method = rhs._method;
		_requestPath = rhs._requestPath;
		_version = rhs._version;
		_host = rhs._host;
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
		_chunkedEnd = rhs._chunkedEnd;
		_perfectBody = rhs._perfectBody;
		_readIndex = rhs._readIndex;
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
	int         bodySize;

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
	_requestPath = token[1];
	_version = token[2];
}

void Request::setFieldLine(std::string& fieldLine)
{
	checkMultipleSpaces(fieldLine);
	std::vector<std::string> token = util::getToken(fieldLine, ": ");
	token[1].erase(0, 1);
	if (token.size() != 2)
		throw 401; // Header error
	if (token[0].empty() || token[1].empty())
		throw 402; // Header Key Value Empty
	if (token[0].find(' ') != std::string::npos)
		throw 403; // Header Key have space
	if (token[0] == "Host") {
		size_t mid = token[1].find(":");
		if (mid == std::string::npos) // Host에 포트정보가 없을 경우, 80을 붙여준다? 이건 고민해봐야함 - kyeonkim
			token[1] += ":80";
		_host = std::string(token[1], 0, mid);
		_port = util::stoui(std::string(token[1], mid + 1, token[1].size() - (mid + 1)));
	}
	if (token[0] == "Content-Type")
	{
		const std::string hash = "boundary=";
		int index = token[1].find(hash);
		if (index != std::string::npos)
			_boundary = "--" + std::string(token[1], index + hash.length(), token[1].size() - (index + hash.length()));
		_contentType = token[1];
	} 
	if (token[0] == "Content-Length")
		_contentLength = token[1];
	if (token[0] == "Transfer-Encoding")
		_transferEncoding = token[1];
	if (token[0] == "Connection")
		_connection = token[1];
	if (token[0] == "X-Secret-Header-For-Test")
		_secretHeader = token[1];
}

void Request::headerParsing(int fd)
{
	// fd 변수는 디버그 용 나중에 리펙토링할 때는 삭제해도 됨 - kyeonkim
	// _headerBuffer.append(buf, size); // 나중에 삭제해도 될 거 같음 - kyeonkim
	// int headerEnd = _headerBuffer.find("\r\n\r\n");
	int headerEnd = _requestBuffer.find("\r\n\r\n"); // 헤더 끝줄 찾기
	if (headerEnd == std::string::npos)
		return ;
// std::cout << BLUE << "testcode "<< "fd : " << fd << "====headerbuff\n" << _headerBuffer.substr(0, _headerBuffer.find("\r\n\r\n")) << RESET << std::endl;
std::cout << BLUE << "testcode "<< "fd : " << fd << "====headerbuff\n" << _requestBuffer.substr(0, _requestBuffer.find("\r\n\r\n")) << RESET << std::endl;
	int endLine = _requestBuffer.find("\r\n");
	std::string requestLine(_requestBuffer, 0, endLine);
	setRequestLine(requestLine);

	int newEndLine;
	endLine += 2;
	while (endLine < headerEnd) { 
		newEndLine = _requestBuffer.find("\r\n", endLine);
		std::string fieldLine(_requestBuffer, endLine, newEndLine - endLine);
		setFieldLine(fieldLine);
		endLine = newEndLine + 2;
	}
	_bodyStartIndex = headerEnd + 4;
	_state = request::CREATE;
}


/*	kyeonkim
	@des 각 서버들의 이름이 여러 개로 들어올 수도 있으므로 loop문을 통해 해당서버를 찾아넘긴다.
		알맞은 서버를 못찾을 경우, 서버들 중에 제일 위쪽([0])에 있는 서버를 넘긴다.
	@return Server*
*/
Server*	Request::findServer()
{
	for (int i = 0; i < _servers.size(); ++i)
	{
		std::vector<std::string> serverName = _servers[i].getServerName();
		for (int j = 0; j < serverName.size();++j)
		{
			if (serverName[j] == _host)
				return &_servers[i];
		}
	}
	return &_servers[0];
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
	// 리펙토링 필요 - kyeonkim
	size_t	requestBufferSize = _requestBuffer.size();
	char*	requestBuffer = const_cast<char *>(_requestBuffer.c_str());
	while (true)
	{
		size_t index = _requestBuffer.find("\r\n", _readIndex);
		if (index != std::string::npos)
		{
			char*	endptr;
			size_t	bodyStart = index + 2;
			size_t	bodySize = std::strtol(requestBuffer + _readIndex, &endptr, HEX);
			if (bodySize == 0)
			{
				if (_requestBuffer.find("\r\n", bodyStart) != std::string::npos)
				{
					if (_location->_clientMaxBodySize.empty() == false)
					{
						std::cerr << BLUE << "maxbodysize: " <<  util::stoui(_location->_clientMaxBodySize) << RESET << std::endl;
						std::cerr << BLUE << "bodySize: " << _perfectBody.size() << std::endl;
						if (_perfectBody.size() > util::stoui(_location->_clientMaxBodySize))
							throw 413;
					}
					_chunkedEnd = true;
				}
				break;
			} 
			else if (bodyStart + bodySize + 2 <= requestBufferSize)//body뒤의 \r\n고려
			{	
				if (_requestBuffer.find("\r\n", bodyStart + bodySize) != bodyStart + bodySize)
					throw 400;
				_perfectBody.append(requestBuffer + bodyStart, bodySize);
				_bodyTotalSize = _perfectBody.size();
				_readIndex = bodyStart + bodySize + 2;//body뒤의 \r\n고려
			}
			else
				break;
		}
		else
			break;
	}
}

void Request::clearRequest()
{
	_location = NULL;
	_state = 0;
	_headerBuffer.clear();
	_requestBuffer.clear();
	_method.clear();
	_requestPath.clear();
	_version.clear();
	_connection.clear();
	_contentType.clear();
	_contentLength.clear();
	_transferEncoding.clear();
	_boundary.clear();
	_chunkedFilename.clear();
	_eventState = 0;
	_bodyStartIndex = 0;
	_readIndex = 0;
	_chunkedEnd = false;
	_bodyTotalSize = 0;
	_perfectBody.clear();
	_secretHeader.clear();
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

void Request::setPerfectBody(std::string& body)
{
	_perfectBody = body;
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

const std::string& Request::getContentLength() const
{
	return _contentLength;
}

const std::string& Request::getHost() const
{
	return _host;
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
	return _requestPath;
}

int Request::getState() const
{
	return _state;
}

const std::string& Request::getBuffer() const
{
	return _requestBuffer;
}

const Server* Request::getServer() const
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

const std::string& Request::getChunkedFilename()
{
	return _chunkedFilename;
}

int Request::getChunkedEnd() const
{
	return _chunkedEnd;
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


const std::string& Request::getSecretHeader() const
{
	return _secretHeader;
}

void Request::setChunkedEnd(bool set)
{
	_chunkedEnd = set;
}

void Request::setServer(Server* server)
{
	_server = server;
}