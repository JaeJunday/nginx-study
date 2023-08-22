#include "Client.hpp"
#include "Request.hpp"
 
// extern int gcount; // -- delete

// Client::Client(int kq)
// : _version("HTTP/1.1"), 
//   _stateCode("200"), 
//   _reasonPhrase("OK"),
//   _serverName("My Server"),
//   _contentType("text/html"),
//   _contentLength(0),
//   _kq(kq),
//   _request(NULL)
// {
//     /* Constructor Implementation */
// }


Client::Client(Request* request, int kq, int socketFd) 
: _request(request),
_socketFd(socketFd),
_pid(-2),
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
	_readFd[0] = -2;
	_readFd[1] = -2;
	_writeFd[0] = -2;
	_writeFd[1] = -2;
}

Client::Client(const Client& src)
{
    /* Copy Constructor Implementation */
}

Client& Client::operator=(Client const& rhs)
{
	if (this != &rhs)
	{
		delete _request;
		_request = new Request(rhs.getReq());
	}
}

// Request 제거해야함
Client::~Client()
{
	if (_pid != -2)
	{
		closePipeFd();
		deletePidEvent();
		kill(_pid, SIGKILL); // 파이프에 쓰다가 에러 throw하는 상황으로 잘 죽나 체크하기 jaejkim
		_pid = -2;
	}
	delete _request;
    /* Destructor Implementation */
}

/*
Client& Client::operator=(Client const& rhs)
{
    if (this != &rhs)
    {
         Assignment Operator Implementation
    }
    return *this;
}
*/

std::string Client::getDate()
{
    std::time_t now = std::time(NULL);
    char timeStamp[TIME_SIZE];
    std::strftime(timeStamp, sizeof(timeStamp), "%a, %d %b %Y %H:%M:%S GMT", std::localtime(&now));
    return (timeStamp);
}

// 15:53:14 : 0.0.0.0 GET HTTP/1.1 200 Ok ./public 2259
void Client::stamp() const
{
	std::string color;
	if (_stateCode >= 400)
		color = RED;
	else 
		color = GREEN;
    std::cerr << color << getDate() << " : "<< _request->getIp() << " " << _request->getMethod() << " " << _request->getVersion() << " "<< _stateCode << " " << _reasonPhrase << RESET << std::endl;
}

const std::stringstream& Client::getBuffer() const
{
    return _responseBuffer;
}

std::string Client::findLocationPath() const
{
	const Server server = _request->getServer();
	const std::vector<Location>& locations = server.getLocations();
	std::string result = _request->getRequestUrl();
	Location location;
	int length = 0;

	if (result.empty())
	{
		// 경로가 없는 경우 errorcode
	}
	for (int i = 0; i < locations.size(); ++i) {
		int pathLength = locations[i]._path.length();
		if (_request->getRequestUrl().compare(0, pathLength, locations[i]._path) == 0)
		{
			if (length < pathLength)
			{
				length = pathLength;
				location = locations[i];
				_request->setLocation(const_cast<Location *>(&_request->getServer().getLocation(i)));
			}
		}
 	}
	if (!location._root.empty())
	{
		result.erase(0, length);
		result = location._root + result;
	}
	else if (!server.getRoot().empty())	
	{
		result.erase(0, length);
		result = server.getRoot() + result;
	}
	if (result.size() > 1 && result[result.size() - 1] == '/')
		result.erase(result.size() - 1, 1);
	return result;
}

void Client::checkLimitExcept() const
{
	std::vector<std::string> limit = _request->getLocation()->_limitExcept;
	int limitSize = limit.size();
	if (limitSize)
	{	
		int i = 0;
		while(i < limitSize)
		{
			if (limit[i] == _request->getMethod())
				break;
			++i;
		}
		if (i == limitSize)
		{
			std::cerr << B_RED << "testcode " << "Error : no accept method." << RESET << std::endl;
			throw 405;
		}
	}
}

std::string Client::findContentType(const std::string& filePath)
{
	std::vector<std::string> filename = util::getToken(filePath, ".");
	std::string fileExtension;
	size_t lastElement = filename.size();
    if (lastElement >= 1)
		fileExtension = filename[filename.size() - 1];
	else
		return "text/plain";

	std::string fileType[] = {"html", "css", "js", "json", "jpeg", "jpg", "png", "gif", "bmp", "webp", "mpeg", "wav", "ogg", "mp4", "webm", "pdf", "zip", "csv"};
    std::string inputType[] = {"text/html", "text/css", "text/javascript", "application/json", "image/jpeg", "image/jpeg", "image/png", "image/gif", "image/bmp", "image/webp", "audio/mpeg", "audio/wav", "audio/ogg", "video/mp4", "video/webm", "application/pdf", "application/zip", "text/csv"};

	for (int i = 0; i < fileType->size(); ++i)
	{
		if (fileExtension == fileType[i])
			return inputType[i];
	}
	return "";
}

int Client::getKq() const
{
	return _kq;
}

Request& Client::getReq() const
{
	return *_request;
}

int Client::getStateCode() const
{
	return _stateCode;
}

int Client::getSocket() const
{
	return _socketFd;
}

void Client::deleteReadEvent()
{
    struct kevent event;

	EV_SET(&event, _socketFd, EVFILT_READ, EV_DELETE, 0, 0, this);
	if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
	{
		std::cerr << B_RED << "testcode _socketFd : " << _socketFd << RESET << std::endl;
		std::cerr << "invalid Read event delete" << std::endl;
		std::cerr << B_RED << "testcode read strerror : " << strerror(errno) << RESET << std::endl;
	}
}

// void Client::deleteReadEvent()
void Client::deleteWriteEvent()
{
    struct kevent event;

	EV_SET(&event, _socketFd, EVFILT_WRITE, EV_DELETE, 0, 0, this);
	if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
	{
		std::cerr << "invalid Write event delete" << std::endl;
		std::cerr << B_RED << "testcode write strerror: " << strerror(errno) << RESET << std::endl;
	}
}


void Client::deletePidEvent()
{
    struct kevent event;

	std::cerr << B_RED << "testcode deletePidEvent()===" << RESET << std::endl;
	EV_SET(&event, _pid, EVFILT_PROC, EV_DELETE, NOTE_EXIT, 0, this);
	if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
		std::cerr << "invalid Pid event delete" << std::endl;
}

// 소켓 전용 event setter
void Client::addEvent(int fd, int filter)
{
    struct kevent event;

    if (filter == EVFILT_READ)
    {
		std::cerr << GREEN << "Read eventSet" << RESET << std::endl;
        EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, this);
        if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
            std::cerr << "invalid Read event set" << std::endl;
    }
    else if (filter == EVFILT_WRITE)
    {
		std::cerr << GREEN << "Write eventSet" << RESET << std::endl;
        EV_SET(&event, fd, EVFILT_WRITE, EV_ADD, 0, 0, this);
        if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
            std::cerr << "invalid Write event set" << std::endl;
    }
	else if (filter == EVFILT_PROC)
	{
		std::cerr << GREEN << "Pid eventSet" << RESET << std::endl;
		EV_SET(&event, fd, EVFILT_PROC, EV_ADD, NOTE_EXIT, 0, this);
        if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
            std::cerr << "invalid Pid event set" << std::endl;
	}
}

void Client::clearClient()
{
	_request->clearRequest();
	// if (_writeFd[0] != -2 && _pid == -2)
	// 	close(_writeFd[0]);
	// if (_writeFd[1] != -2 && _pid == -2)
	// 	close(_writeFd[1]);
	// if (_readFd[0] != -2 && _pid == -2)
	// 	close(_readFd[0]);
	// if (_readFd[1] != -2 && _pid == -2)
	// 	close(_readFd[1]);
	// _writeFd[0] = -2;
	// _writeFd[1] = -2;
	// _readFd[0] = -2;
	// _readFd[1] = -2;
	// _pid = -2;
	_chunkedFilename.clear();
	_stateCode = 200;
	_reasonPhrase.clear();
	_contentType.clear();
	_contentLength = 0;
	_responseBuffer.str("");
	_convertRequestPath.clear();
	_writeIndex = 0;
	_sendIndex = 0;
	_responseStr.clear();
}

int Client::getWriteFd() const
{
	return _writeFd[1];
}

int Client::getReadFd() const
{
	return _readFd[0]; 
}

void Client::handleRequest(struct kevent* tevent, char* buffer)
{
	_request->setBuffer(buffer, tevent->data);
	if (_request->getState() == request::READY) // header 생성
	{
		_request->headerParsing(buffer, tevent->data, tevent->ident);
	}
	if (_request->getState() == request::CREATE)
	{	
		_convertRequestPath = findLocationPath();
		checkLimitExcept();
		if (_request->getMethod() == "POST" || _request->getMethod() == "PUT")
			initCgi();
		_request->setState(request::DONE);
	}	
	if (_request->getState() == request::DONE) // response body 생성
	{
		handleResponse(tevent);
	}
}

void Client::handleResponse(struct kevent *tevent)
{
	if (_request->getTransferEncoding() == "chunked")
	{
		_request->parseChunkedData(this);
	}
	else if (_request->getBuffer().size() - _request->getBodyIndex()  == util::stoui(_request->getContentLength()))
	{
		// if (_request->getMethod() == "POST" && (_request->getContentLength() == 0 || _request->getBuffer().size() == 0))
		// 	throw 405;
		if (_request->getMethod() == "GET")
		{
			getProcess();
		}
		else if (_request->getMethod() == "POST" || _request->getMethod() == "PUT")
		{
			postProcess();
			return;
		}
		else if (_request->getMethod() == "DELETE")
		{
			deleteProcess();
		}
		// deleteEvent();
std::cerr << "fd: " << _socketFd << RED << "in handle response funtion delete event" << RESET << std::endl;
		deleteReadEvent();
		addEvent(tevent->ident, EVFILT_WRITE);
		_request->setEventState(EVFILT_WRITE);
	}
}

void Client::closePipeFd()
{
	// if (_writeFd[0] != -2)
	// {
	// 	close(_writeFd[0]);
	// 	_writeFd[0] = -2;
	// }
	if (_writeFd[1] != -2)
	{
		close(_writeFd[1]);
		// _writeFd[1] = -2;
	}
	if (_readFd[0] != -2)
	{
		close(_readFd[0]);
		// _readFd[0] = -2;
	}
	// if (_readFd[1] != -2)
	// {
	// 	close(_readFd[0]);
	// 	_readFd[1] = -2;
	// }
}

bool Client::sendData(struct kevent& tevent)
{
std::cerr << "fd: " << tevent.ident <<  "==============================Send data==============================" << std::endl;
std::cerr << B_CYAN << "testcode ===" << "tevent.data : " << tevent.data << RESET << std::endl;
	size_t responseBufferSize = _responseStr.size();
	size_t sendBufferSize = std::min(responseBufferSize - _sendIndex, (size_t)tevent.data);
// std::cerr << BLUE << "_sendIndex before:" << _sendIndex << RESET << std::endl;
	ssize_t byteWrite = send(tevent.ident, _responseStr.c_str() + _sendIndex, sendBufferSize, 0);
	// send실패에는 소켓도 닫아야됨, 프로세스 종료
	// 에러코드일때는 소켓은 안닫고 프로세스 종료
	if (_stateCode >= 400) // 에러도 소켓은 살려놓는다. 
	{
		if (_pid != -2)
		{
			closePipeFd();
			deletePidEvent();
			kill(_pid, SIGKILL); // 파이프에 쓰다가 에러 throw하는 상황으로 잘 죽나 체크하기 jaejkim
			_pid = -2;
		}
	}
	if (byteWrite == -1 || _stateCode == 405) // send fail
	{
		std::cerr << B_RED << strerror(errno) << RESET << std::endl;
		std::cerr << RED << "event ident: " <<  tevent.ident << RESET << std::endl;
		clearClient();
		std::cerr << B_BG_CYAN << "끊겼어용" << RESET << std::endl;
		close(tevent.ident);
		delete this;
		return false;
	}
// std::cerr << GREEN << "testcode : " << "send code: " << _stateCode << RESET << std::endl;
	_sendIndex += byteWrite;
// std::cerr << YELLOW << _responseBuffer.str() << RESET << std::endl;
// std::cerr << GREEN << "buffer length :" << responseBufferSize << RESET << std::endl;
// std::cerr << GREEN << "write byte count :" << byteWrite << RESET << std::endl;
// std::cerr << RED << "_sendIndex after:" << _sendIndex << RESET << std::endl;
	if (_sendIndex == responseBufferSize)
	{
		// deleteEvent();
		deleteWriteEvent();
		clearClient();
		addEvent(tevent.ident, EVFILT_READ);
		_request->setEventState(EVFILT_READ);
		std::cerr << GREEN << "testcode " << "===========send clear==============" << RESET << std::endl;
	}
	return true;
}

