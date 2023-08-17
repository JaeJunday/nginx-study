#include "Client.hpp"
#include "Request.hpp"
#include "include/Color.hpp"
 
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

	std::string fileType[] = {"html", "css", "js", "json", "jpeg", "jpg", "png", "gif", "bmp", "webp", "mpeg", "wav", "ogg", "mp4", "webm", "pdf", "zip"};
    std::string inputType[] = {"text/html", "text/css", "text/javascript", "application/json", "image/jpeg", "image/jpeg", "image/png", "image/gif", "image/bmp", "image/webp", "audio/mpeg", "audio/wav", "audio/ogg", "video/mp4", "video/webm", "application/pdf", "application/zip"};

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

void Client::deleteEvent()
{
    struct kevent event;

    if (_request->getEventState() == EVFILT_READ)
    {
        EV_SET(&event, _socketFd, EVFILT_READ, EV_DELETE, 0, 0, this);
        kevent(_kq, &event, 1, NULL, 0, NULL);
    }
    else if (_request->getEventState() == EVFILT_WRITE)
    {
        EV_SET(&event, _socketFd, EVFILT_WRITE, EV_DELETE, 0, 0, this);
        kevent(_kq, &event, 1, NULL, 0, NULL);
    }
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
            std::cerr << "invalid Read event set 1" << std::endl;
    }
    else if (filter == EVFILT_WRITE)
    {
		std::cerr << GREEN << "Write eventSet" << RESET << std::endl;
        EV_SET(&event, fd, EVFILT_WRITE, EV_ADD, 0, 0, this);
        if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
            std::cerr << "invalid Write event set 1" << std::endl;
    }
}

void Client::clearClient()
{
// std::cerr << YELLOW << "clear!" << RESET << std::endl;
	_request->clearRequest();
	if (_writeFd[0] != -2)
		close(_writeFd[0]);
	if (_writeFd[1] != -2)
		close(_writeFd[1]);
	if (_readFd[0] != -2)
		close(_readFd[0]);
	if (_readFd[1] != -2)
		close(_readFd[1]);
	_writeFd[0] = -2;
	_writeFd[1] = -2;
	_readFd[0] = -2;
	_readFd[1] = -2;
	_pid = -2;
	_chunkedFilename.clear();
	_stateCode = 200;
	_reasonPhrase.clear();
	_contentType.clear();
	_contentLength = 0;
	_responseBuffer.str("");
	_convertRequestPath.clear();
	_writeIndex = 0;
	_sendIndex = 0;
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
		_request->headerParsing(buffer, tevent->data);
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
		while (true) // 한번 돌때 완성된 문자열 하나씩 처리
		{
			_request->parseChunkedData(this);
			int chunkedState = _request->getChunkedState();
			// std::cerr << B_RED << "testcode " << "chunkedState : " << chunkedState<< RESET << std::endl;
			if (chunkedState == chunk::CONTINUE)
				continue;
			else if (chunkedState == chunk::END)
			{
				std::cerr << RED << "testcode: " << "chunked::end" << RESET << std::endl;
				break;
			}
			else if (chunkedState == chunk::INCOMPLETE_DATA)
			{
				// std::cerr << RED << "testcode" << "chunked::IN DATA" << RESET << std::endl;
				return;
			}
		}
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
		deleteEvent();
		addEvent(tevent->ident, EVFILT_WRITE);
		_request->setEventState(EVFILT_WRITE);
	}
}

bool Client::sendData(struct kevent& tevent)
{
std::cerr << "==============================Send data==============================" << std::endl;
// std::cerr << B_CYAN << "testcode " << "tevent.data: " << tevent.data << RESET << std::endl;
	size_t responseBufferSize = _responseBuffer.str().size();
	size_t sendBufferSize = std::min(responseBufferSize - _sendIndex, (size_t)tevent.data);
// std::cerr << BLUE << "_sendIndex before:" << _sendIndex << RESET << std::endl;
	size_t byteWrite = send(tevent.ident, _responseBuffer.str().c_str() + _sendIndex, sendBufferSize, 0);
	if (byteWrite < 0 || _stateCode >= 400)
	{
		clearClient();
		std::cerr << B_BG_CYAN << "끊겼어용" << RESET << std::endl;
		close(_socketFd);
		delete this;
		return false;
	}
	_sendIndex += byteWrite;
// std::cerr << YELLOW << client->getBuffer().str().c_str() << RESET << std::endl;
// std::cerr << GREEN << "buffer length :" << responseBufferSize << RESET << std::endl;
// std::cerr << GREEN << "write byte count :" << byteWrite << RESET << std::endl;
// std::cerr << RED << "_sendIndex after:" << _sendIndex << RESET << std::endl;
// std::cerr << GREEN << "testcode : " << "send code: " << _stateCode << RESET << std::endl;
	if (_sendIndex == responseBufferSize)
	{
		deleteEvent();
		clearClient();
		addEvent(tevent.ident, EVFILT_READ);
		_request->setEventState(EVFILT_READ);
		// std::cerr << GREEN << "testcode " << "===========send clear==============" << RESET << std::endl;
	}
	else
	{
		// std::cerr << GREEN << "testcode " << "bytewrite fail" << RESET << std::endl;
	}
	return true;
}

