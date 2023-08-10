#include "Operation.hpp"

Operation::~Operation()
{
	std::map<int, Request*>::iterator it;
	if (!_requests.empty())
	{
		for (it = _requests.begin(); it != _requests.end(); ++it)
			delete it->second;
	}
}

void Operation::setServer(const Server& server) 
{
	_servers.push_back(server);
}

const std::vector<Server>& Operation::getServers() const
{
	return _servers;
}

int Operation::createBoundSocket(std::string listen)
{
	int socketFd = socket(AF_INET, SOCK_STREAM, FALLOW);
	if (socketFd == -1)
		throw std::logic_error("Error: Socket creation failed");
	sockaddr_in serverAddr;
	int optval = 1;
	memset((char*)&serverAddr, 0, sizeof(sockaddr_in));
	std::vector<std::string> ipPort = util::getToken(listen, ":");
	uint32_t ip = 0x0000000; 
	uint32_t port = 80;
	if (ipPort.size() == 1)
		port = util::stoui(ipPort[0]);
	else if (ipPort.size() == 2)
	{
		ip = util::convertIp(ipPort[0]); 
		port = util::stoui(ipPort[1]);
	}
	// default ip address
		std::cerr << "http://";
		if (ipPort.size() == 1)
			std::cerr << "localhost" << ":" << ipPort[0];
		else if (ipPort.size() == 2)
			std::cerr << ipPort[0] << ":" << ipPort[1]; 
		std::cerr << std::endl;
	// ip address
	serverAddr.sin_family = AF_INET; 
	serverAddr.sin_addr.s_addr = htonl(ip);
	serverAddr.sin_port = htons(port);
	setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (bind(socketFd, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1)
		throw std::logic_error("Error: Socket bind failed");
	return socketFd;
}
/*
	@des 서버가 여러 개일 경우, 해당 서버가 몇 번째 인덱스에 있는지 찾아서 해당 인덱스 반환
	@return serverIndex(int)
*/
int Operation::findServer(uintptr_t ident) const
{
	for (int i = 0; i < _servers.size(); ++i)
		if (static_cast<uintptr_t>(_servers[i].getSocket()) == ident)
			return i;
	return -1;
}

void Operation::start() {
	// 서버 시작 로직을 구현합니다.
	/*
		@des 서버마다 소켓을 하나 만들어서 Set, 그리고 서버는 Listen 상태로 만들어둠
	*/
	for(int i = 0; i < _servers.size(); ++i)
	{
		try {
			std::string number = _servers[i].getValue(server::LISTEN);
			int socketFd = createBoundSocket(number);
			_servers[i].setSocket(socketFd);
			fcntl(_servers[i].getSocket(), F_SETFL, O_NONBLOCK);
			if (listen(_servers[i].getSocket(), SOMAXCONN) == -1) // SOMAXCONN == 128
				throw std::logic_error("Error: Listen failed");
		} catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
			continue;
		}
	}
	int kq, nev;
	kq = kqueue();
	struct kevent event, events[10];
	struct kevent tevent;	 /* Event triggered */
	/*
		@des 각 서버마다 READ 이벤트를 걸어둠
	*/
	for(size_t i = 0; i < _servers.size(); ++i)
	{
		EV_SET(&event, _servers[i].getSocket(), EVFILT_READ, EV_ADD, 0, 0, nullptr);
		kevent(kq, &event, 1, NULL, 0, NULL);
	}
	/*
		@des 무한 loop 를 돌며 서버로 들어오는 이벤트를 감지, 감지한 이벤트에 따라 처리
	*/
	while (true)
	{
		nev = kevent(kq, NULL, 0, &tevent, 1, NULL); // EVFILT_READ, EVFILT_WRITE 이벤트가 감지되면 이벤트 감지 개수를 반환
		if (nev == -1)
			throw std::runtime_error("Error: kevent error");
		int serverIndex = findServer(tevent.ident);
		if (serverIndex >= 0) // 서버일 경우
			acceptClient(kq, serverIndex);
		else // 클라이언트일 경우
		{
			if (tevent.filter == EVFILT_READ)
			{
				char* buffer = new char[tevent.data];
				ssize_t bytesRead = recv(tevent.ident, buffer, tevent.data, 0);
				Request *req = static_cast<Request*>(tevent.udata);
				// std::cout << RED << "testcode " << "recv, detect socket fd : " << tevent.ident << RESET << std::endl;
				// write(1, buffer, tevent.data);
				// std::cerr << std::endl;
				if (bytesRead == false || req->getConnection() == "close")
				{
					// std::cerr << "###################### client end ##############################" << std::endl;
					close(req->getSocket());
					delete req;
					_requests.erase(tevent.ident);
				}
				else
				{
					try{
						req->setBuffer(buffer, tevent.data);
						if (req->getState() == request::READY) // header 생성
							req->headerParsing(buffer, tevent.data);
						if (req->getState() == request::CREATE) // response 생성
							req->makeResponse(kq);
						if (req->getState() == request::DONE) // response body 생성
							this->handleResponse(req, kq, &tevent, buffer);
					} catch(const int errnum) {
						AResponse* resError = new Error(req, kq);
						dynamic_cast<Error *>(resError)->makeErrorPage(errnum);
						req->setEventState(event::WRITE);
						EV_SET(&tevent, tevent.ident, EVFILT_WRITE, EV_ADD, 0, 0, resError);
						kevent(kq, &tevent, 1, NULL, 0, NULL);
					} catch(const std::exception& e) {
						std::cerr << "exception error : " << e.what() << std::endl;
					}
				}
				delete[] buffer;
			}
			else if (tevent.filter == EVFILT_WRITE)
			{
				if (tevent.udata)
				{
					AResponse *res = static_cast<AResponse*>(tevent.udata);
					if (tevent.ident == res->getRequest()->getSocket())
						sendData(tevent);
				}
			}
		}
	}
}

void Operation::handleResponse(Request* req, int kq, struct kevent *tevent, char* buffer)
{
// chunkedBuffer 를 파싱하여 body길이가 정확할 경우 substr을 통해 body 데이터를 잘라서 (createResponse) writeFd에 써줌
// index로 파싱한 부분의 시작지점을 가지고 있음 (즉, body size 의 시작지점)
// 0/r/n/r/n 만나고 나서  파이프 다 쓰면 close(writeFd)
	// buffer의 len을 읽어서 숫자를 보고 
	// body index 부터 
	if (req->getTransferEncoding() == "chunked")
	{
		while(true)
		{
			req->parseChunkedData();
			int chunkedState = req->getChunkedState();
			// std::cerr << B_RED << "testcode " << "chunkedState : " << chunkedState<< RESET << std::endl;
			if (chunkedState == chunk::END)
			{
				std::cerr << RED << "testcode" << "chunked::end" << RESET << std::endl;
				EV_SET(tevent, tevent->ident, EVFILT_WRITE, EV_ADD, 0, 0, req->getResponse());
				kevent(kq, tevent, 1, NULL, 0, NULL);
				req->setEventState(event::WRITE);
				break;
			}
			else if (chunkedState == chunk::INCOMPLETE_DATA)
			{
				// std::cerr << RED << "testcode" << "chunked::IN DATA" << RESET << std::endl;
				return;
			}
		}

	}
	else if (req->getBuffer().size() - req->getBodyIndex()  == req->getContentLength())
	{
		if (req->getMethod() == "POST" && (req->getContentLength() == 0 || req->getBuffer().size() == 0))
			throw 405;
		// AResponse* response = selectMethod(req, kq);
		req->getResponse()->createResponse();
		EV_SET(tevent, tevent->ident, EVFILT_WRITE, EV_ADD, 0, 0, req->getResponse());
		kevent(kq, tevent, 1, NULL, 0, NULL);
		req->setEventState(event::WRITE);
	}
}

void Operation::acceptClient(int kq, int index)
{
	int				requestFd;
	sockaddr_in		requestAddr;
	socklen_t		requestLen;
	// struct kevent	revent;
	std::cout << GREEN << "testcode" << "=================== ACCEPT =========================" << RESET << std::endl;	
	requestFd = accept(_servers[index].getSocket(), reinterpret_cast<struct sockaddr*>(&requestAddr), &requestLen);
	if (requestFd == -1)
		throw std::logic_error("Error: Accept failed");
	fcntl(requestFd, F_SETFL, O_NONBLOCK);
	Request *request = new Request(requestFd, _servers[index]);
	_requests.insert(std::make_pair(requestFd, request));
	util::setReqEvent(request, kq, EVFILT_READ);
}

void Operation::sendData(struct kevent& tevent)
{
	AResponse* response = static_cast<AResponse*>(tevent.udata);
	size_t byteWrite = send(tevent.ident, response->getBuffer().str().c_str(), response->getBuffer().str().length(), 0);
	std::cerr << "==============================response data==============================" << std::endl;
	std::cerr << response->getBuffer().str().c_str() << std::endl;
	// std::cout << "buffer length :" << response->getBuffer().str().length() << std::endl;
	//std::cout << "write byte count :" << byteWrite << std::endl;
	std::cerr << GREEN << "testcode : " << "send code: " << response->getStateCode() << RESET << std::endl;
	if (byteWrite == response->getBuffer().str().length())
	{
		response->getRequest()->clearRequest();
		util::setReqEvent(response->getRequest(), response->getKq(), event::READ);
		delete response;
		std::cout << GREEN << "testcode " << "send clear" << RESET << std::endl;	
	}
	else
		std::cout << GREEN << "testcode" << "bytewrite fail" << RESET << std::endl;
}