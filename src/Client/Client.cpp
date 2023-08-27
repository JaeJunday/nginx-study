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

	// client->addEvent(socketFd, EVFILT_READ);
	addSocketReadEvent();
	// client->addEvent(socketFd, EVFILT_TIMER); // 타이머 이벤트 추가 - kyeonkim
	addTimerEvent();
	// _request->setEventState(EVFILT_READ);
}

// Client::Client(const Client& src)
// {}
/*
	[delete] - kyeonkim
	- 현재 클라이언트 대입 쓰고 있음.
	- 쓰고 있는데 request 대입하고 있음.
	- 다른 것도 넣어줘야함. 근데 신기하게 왜 잘 동작함?
	- 심지어 리턴도 없음 ㅋㅋㅋ
	- 근데 잘보니 안씀 ㅋㅋㅋ 삭제해도됨 ㅋㅋㅋ
*/
// Client& Client::operator=(Client const& rhs)
// {
// 	if (this != &rhs)
// 	{
// 		delete _request;
// 		_request = new Request(rhs.getReq());
// 	}
// }

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

void Client::stamp() const
{
	std::string color;

	if (_stateCode >= 400)
		color = RED;
	else 
		color = GREEN;
    std::cerr << color << util::getDate() << " : "<< _request->getHost() << " " << _request->getMethod() << " " << _request->getVersion() << " "<< _stateCode << " " << _reasonPhrase << RESET << std::endl;
}

void Client::clearClient()
{
	_request->clearRequest();
	_chunkedFilename.clear();
	_stateCode = 200;
	_reasonPhrase.clear();
	_contentType.clear();
	_contentLength = 0;
	_responseBuffer.str("");
	_writeIndex = 0;
	_sendIndex = 0;
	_responseStr.clear();
}

// [Refectoring required] - semikim
void Client::handleResponse(const struct kevent &tevent)
{
	if (_request->getTransferEncoding() == "chunked")
	{
		_request->parseChunkedData(this);
	}
	else if (_request->getBuffer().size() - _request->getBodyIndex()  == util::stoui(_request->getContentLength()))
	{
		if (_request->getMethod() == "POST" || _request->getMethod() == "PUT")
		{
			postProcess();
			return;
		}
		if (_request->getMethod() == "GET")
		{
			if (isFilePy(_request->getConvertRequestPath()))
			{
				getCgi();
				return;
			}
			else
				getProcess();
		}
		else if (_request->getMethod() == "DELETE")
		{
			deleteProcess();
		}
		// deleteEvent();
		deleteReadEvent();
		// addEvent(tevent->ident, EVFILT_WRITE);
		addSocketWriteEvent();
		// _request->setEventState(EVFILT_WRITE);
	}
}

void Client::closePipeFd()
{
	if (_writeFd[1] != INIT_PIPE)
		close(_writeFd[1]);
	if (_readFd[0] != INIT_PIPE)
		close(_readFd[0]);
}

bool Client::sendData(const struct kevent& tevent)
{
	size_t responseBufferSize = _responseStr.size();;
	size_t sendBufferSize = std::min(responseBufferSize - _sendIndex, (size_t)tevent.data);
	ssize_t byteWrite = send(tevent.ident, _responseStr.c_str() + _sendIndex, sendBufferSize, 0);
	if (_stateCode >= 400)
	{
		if (_pid != -2)
		{
			closePipeFd();
			deletePidEvent();
			kill(_pid, SIGKILL);
			_pid = -2;
		}
	}
	if (byteWrite <= 0 || _stateCode == 405)
		return false;
	_sendIndex += byteWrite;
	if (_sendIndex == responseBufferSize)
	{
		deleteWriteEvent();
		clearClient();
		// addEvent(tevent.ident, EVFILT_READ);
		addSocketReadEvent();
		// _request->setEventState(EVFILT_READ);
	}
	return true;
}
