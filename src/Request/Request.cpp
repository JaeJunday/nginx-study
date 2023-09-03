#include "Request.hpp"
#include "Client.hpp"

Request::Request(std::vector<Server>& servers)
	:_servers(servers),
	_port(0), 
	_state(request::READY),
	_bodyStartIndex(0), 
	_bodyTotalSize(0),
	_chunkedEnd(false),
	_readIndex(0)
{}

Request::Request(const Request& request)
	: _server(request._server)
{
	*this = request;
}

Request::~Request(){}

Request& Request::operator=(const Request& rhs)
{
	if (this != &rhs)
	{
		_servers = rhs._servers;
		_server = rhs._server;
		_location = rhs._location;
		_headerBuffer = rhs._headerBuffer;
		_requestBuffer = rhs._requestBuffer;
		_method = rhs._method;
		_requestPath = rhs._requestPath;
		_version = rhs._version;
		_host = rhs._host;
		_connection = rhs._connection;
		_contentType = rhs._contentType;
		_contentLength = rhs._contentLength;
		_transferEncoding = rhs._transferEncoding;
		_boundary = rhs._boundary;
		_requestBody = rhs._requestBody;
		_secretHeader = rhs._secretHeader;
		_convertRequestPath = rhs._convertRequestPath;
		_port = rhs._port;
		_state = rhs._state;
		_bodyStartIndex = rhs._bodyStartIndex;	
		_bodyTotalSize = rhs._bodyTotalSize;
		_chunkedEnd = rhs._chunkedEnd;
		_readIndex = rhs._readIndex;
	}
	return *this;	
}

void Request::handleRequest(const struct kevent& tevent, char* buffer)
{
	Client* client = reinterpret_cast<Client*>(tevent.udata);
	
	_requestBuffer.append(buffer, tevent.data);
	delete[] buffer;
	if (_state == request::READY)
		parsingHeader();
	if (_state == request::CREATE)
	{	
		_server = findServer();
		_convertRequestPath = findLocationPath();
		checkLimitExcept();
		if (_method == "POST" || _method == "PUT")
			client->handlePostCgi();
		_state = request::DONE;
	}	
	if (_state == request::DONE)
		client->handleResponse();
}

void Request::parsingHeader()
{
	size_t headerEnd = _requestBuffer.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return ;
	size_t endLine = _requestBuffer.find("\r\n");
	std::string requestLine(_requestBuffer, 0, endLine);
	setRequestLine(requestLine);
	endLine += 2;
	size_t newEndLine;
	while (endLine < headerEnd) 
	{ 
		newEndLine = _requestBuffer.find("\r\n", endLine);
		std::string fieldLine(_requestBuffer, endLine, newEndLine - endLine);
		setFieldLine(fieldLine);
		endLine = newEndLine + 2;
	}
	_bodyStartIndex = headerEnd + 4;
	_state = request::CREATE;
}

Server*	Request::findServer()
{
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		std::vector<std::string> serverName = _servers[i].getServerName();
		for (size_t j = 0; j < serverName.size();++j)
		{
			if (serverName[j] == _host)
				return &_servers[i];
		}
	}
	return &_servers[0];
}

std::string Request::findLocationPath()
{
	const std::vector<Location>* locations = _server->getLocations();
	std::string path = _requestPath;
	size_t length = 0;

	for (size_t i = 0; i < locations->size(); ++i) 
	{
		size_t pathLength = (*locations)[i]._path.length();
		if (path.compare(0, pathLength, (*locations)[i]._path) == 0 && length < pathLength)
		{
			length = pathLength;
			_location = (*locations)[i];
		}
 	}
	if (length == 0)
		throw 400;
	if (!_location._root.empty())
	{
		if (length != 1) 
			path.erase(0, length);
		path = _location._root + path;
	}
	else if (!_server->getRoot().empty())
	{
		if (length != 1)
			path.erase(0, length);
		path = _server->getRoot() + path;
	}
	size_t pathLength = path.length();
	if (pathLength > 0 && path[pathLength - 1] == '/')
		path.erase(pathLength - 1, 1);
	return path;
}

void Request::checkLimitExcept() const
{
	std::vector<std::string> limit = _location._limitExcept;
	int limitSize = limit.size();

	if (limitSize)
	{	
		int i = 0;
		while(i < limitSize)
		{
			if (limit[i] == _method)
				break;
			++i;
		}
		if (i == limitSize)
			throw 405;
	}
	else
	{
		if (_method == "DELETE")
			throw 405;
	}
}

void Request::clearRequest()
{
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
	_bodyStartIndex = 0;
	_readIndex = 0;
	_chunkedEnd = false;
	_bodyTotalSize = 0;
	_requestBody.clear();
	_secretHeader.clear();
	_convertRequestPath.clear();
}
