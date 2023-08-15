#include "Client.hpp"
#include "Request.hpp"
#include "include/Color.hpp"
 
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
_writeIndex(0)
{
	_readFd[0] = -1;
	_readFd[1] = -1;
	_writeFd[0] = -1;
	_writeFd[1] = -1;
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
	std::string limit = _request->getLocation()->_limitExcept;
	if (!limit.empty())
	{	
		std::vector<std::string> allowMethod = util::getToken(limit, " ");
		int limitSize = allowMethod.size();
		if (limitSize)
		{
			int i = 0;
			while(i < limitSize)
			{
				if (allowMethod[i] == _request->getMethod())
					break;
				++i;
			}
			if (i == allowMethod.size())
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
        EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, this);
        if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
            std::cerr << "invalid Read event set 1" << std::endl;
    }
    else if (filter == EVFILT_WRITE)
    {
        EV_SET(&event, fd, EVFILT_WRITE, EV_ADD, 0, 0, this);
        if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
            std::cerr << "invalid Write event set 1" << std::endl;
    }
}

//pipe 전용 event setter
// void Client::addEvent(int filter)
// {
//     struct kevent event;

//     // ADD
//     if (filter == event::READ)
//     {
//         EV_SET(&event, _socketFd, EVFILT_READ, EV_ADD, 0, 0, this);
//         if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
//             std::cerr << "invalid Read event set 2" << std::endl;
//     }
//     else if (filter == event::WRITE && _writeEventFlag == false)
//     {
//         EV_SET(&event, _socketFd, EVFILT_WRITE, EV_ADD, 0, 0, this);
//         if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
//             std::cerr << "invalid Write event set 2" << std::endl;
// 		// state set
// 		_writeEventFlag = true;
//     }
// }

void Client::clearClient()
{
std::cerr << YELLOW << "clear!" << RESET << std::endl;
	_request->clearRequest();
	if (_writeFd[0] != -1)
		close(_writeFd[0]);
	if (_writeFd[1] != -1)
		close(_writeFd[1]);
	if (_readFd[0] != -1)
		close(_readFd[0]);
	if (_readFd[1] != -1)
		close(_readFd[1]);
	_writeFd[0] = -1;
	_writeFd[1] = -1;
	_readFd[0] = -1;
	_readFd[1] = -1;
	_pid = -2;
	_chunkedFilename.clear();
	_stateCode = 200;
	_reasonPhrase.clear();
	_contentType.clear();
	_contentLength = 0;
	_responseBuffer.str("");
	_writeIndex = 0;
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
		findLocationPath();
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
// chunkedBuffer 를 파싱하여 body길이가 정확할 경우 substr을 통해 body 데이터를 잘라서 (createResponse) writeFd에 써줌
// index로 파싱한 부분의 시작지점을 가지고 있음 (즉, body size 의 시작지점)
// 0/r/n/r/n 만나고 나서  파이프 다 쓰면 close(writeFd)
	// buffer의 len을 읽어서 숫자를 보고 
	// body index 부터 
	// Request req = client->getReq();
	// std::cerr << RED << "_request->getTransferEncoding() : " << _request->getTransferEncoding() << RESET << std::endl;
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
	else if (_request->getBuffer().size() - _request->getBodyIndex()  == _request->getContentLength())
	{
		if (_request->getMethod() == "POST" && (_request->getContentLength() == 0 || _request->getBuffer().size() == 0))
			throw 405;
		if (_request->getMethod() == "GET")
		{
			getProcess();
		}
		if (_request->getMethod() == "POST" || _request->getMethod() == "PUT")
		{
			postProcess();
		}
		if (_request->getMethod() == "DELETE")
		{
			deleteProcess();
		}
		// if (_request->getMethod() == "ERROR")
		// {
		// 	errorProcess();
		// }
		// _request->getResponse()->createRe();
		// EV_SET(tevent, tevent->ident, EVFILT_WRITE, EV_ADD, 0, 0, client);
		// kevent(kq, tevent, 1, NULL, 0, NULL);
		// GET, POST, DLETE, ERROR 분기 필요 - kyeonkim
		addEvent(tevent->ident, EVFILT_WRITE);
		_request->setEventState(EVFILT_WRITE);
	}
}