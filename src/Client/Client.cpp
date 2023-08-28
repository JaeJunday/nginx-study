#include "Client.hpp"
#include "Request.hpp"

Client::Client(Request* request, int kq, int socketFd) 
: _request(request),
_socketFd(socketFd),
_pid(INIT_PID),
_version("HTTP/1.1"),
_stateCode(200),
_reasonPhrase("OK"),
_serverName("My Server"),
_contentType("text/html"),
_contentLength(0),
_kq(kq),
_writeIndex(0),
_sendIndex(0)
{
	_readFd[0] = INIT_PIPE;
	_readFd[1] = INIT_PIPE;
	_writeFd[0] = INIT_PIPE;
	_writeFd[1] = INIT_PIPE;
	addSocketReadEvent();
	addTimerEvent();
}

Client::~Client()
{
	if (_pid != INIT_PID)
	{
		closePipeFd();
		deletePidEvent();
		kill(_pid, SIGKILL);
		_pid = INIT_PID;
	}
	deleteTimerEvent(); 
	delete _request;
}

void Client::handleResponse()
{
	if (_request->getTransferEncoding() == "chunked")
		_request->parsingChunkedData();
	else if (_request->getRequestBuffer().size() - _request->getBodyIndex()  == util::stoui(_request->getContentLength()))
	{
		const std::string method = _request->getMethod();
		if (method == "GET")
			handleGet();
		else if (method == "POST" || method == "PUT")
			handlePost();
		else if (method == "DELETE")
			handleDelete();
	}
}

bool Client::sendData(const struct kevent& tevent)
{
	size_t responseBufferSize = _responseStr.size();;
	size_t sendBufferSize = std::min(responseBufferSize - _sendIndex, (size_t)tevent.data);
	ssize_t byteWrite = send(tevent.ident, _responseStr.c_str() + _sendIndex, sendBufferSize, 0);

	if (_stateCode >= 400)
	{
		if (_pid != INIT_PID)
		{
			closePipeFd();
			deletePidEvent();
			kill(_pid, SIGKILL);
			_pid = INIT_PID;
		}
	}
	if (byteWrite <= 0 || _stateCode == 405)
		return false;
	_sendIndex += byteWrite;
	if (_sendIndex == responseBufferSize)
	{
		stamp();
		deleteSocketWriteEvent();
		clearClient();
		addSocketReadEvent();
	}
	return true;
}

void Client::handleEndProcess()
{
	int status;
	
	waitpid(_pid, &status, 0);
	_pid = INIT_PID;
	if (WEXITSTATUS(status) == 1)
	{
		_responseBuffer.str("");
		throw 400;
	}
	deleteSocketReadEvent();
	addSocketWriteEvent();
}

void Client::clearClient()
{
	_request->clearRequest();
	_chunkedFilename.clear();
	_stateCode = 200;
	_reasonPhrase = "OK";
	_contentType.clear();
	_contentLength = 0;
	_responseBuffer.str("");
	_writeIndex = 0;
	_sendIndex = 0;
	_responseStr.clear();
}

void Client::closePipeFd()
{
	if (_writeFd[1] != INIT_PIPE)
		close(_writeFd[1]);
	if (_readFd[0] != INIT_PIPE)
		close(_readFd[0]);
}

void Client::stamp() const
{
	std::string color;

	if (_stateCode >= 400)
		color = RED;
	else 
		color = GREEN;
    std::cerr << color << util::getDate() << " : "<< _request->getHost() << " " << _request->getMethod() << " " << _request->getConvertRequestPath() << " "<< _stateCode << " " << _reasonPhrase << RESET << std::endl;
}
