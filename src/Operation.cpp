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
//------------------------------------------- default ip address
		std::cerr << "http://";
		if (ipPort.size() == 1)
			std::cerr << "localhost" << ":" << ipPort[0];
		else if (ipPort.size() == 2)
			std::cerr << ipPort[0] << ":" << ipPort[1]; 
		std::cerr << std::endl;
//-------------------------------------------
	// ip address
	serverAddr.sin_family = AF_INET; 
	serverAddr.sin_addr.s_addr = htonl(ip);
	serverAddr.sin_port = htons(port);
	setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (bind(socketFd, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1)
		throw std::logic_error("Error: Socket bind failed");
	return socketFd;
}

int Operation::findServer(uintptr_t ident) const
{
	for (int i = 0; i < _servers.size(); ++i)
		if (static_cast<uintptr_t>(_servers[i].getSocket()) == ident)
			return i;
	return -1;
}

void Operation::start() {
	// 서버 시작 로직을 구현합니다.
	// ...
	for(int i = 0; i < _servers.size(); ++i)
	{
		try {
			std::string number = _servers[i].getValue(server::LISTEN);
			int index = createBoundSocket(number);
			_servers[i].setSocket(index);
			// _servers[i].setSocket(createBoundSocket(_servers[i].getValue(server::LISTEN)));
			fcntl(_servers[i].getSocket(), F_SETFL, O_NONBLOCK);
			if (listen(_servers[i].getSocket(), SOMAXCONN) == -1)
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
	
	for(size_t i = 0; i < _servers.size(); ++i)
	{
		EV_SET(&event, _servers[i].getSocket(), EVFILT_READ, EV_ADD, 0, 0, nullptr);
		kevent(kq, &event, 1, NULL, 0, NULL);
	}
	// loop
	while (true)
	{
		nev = kevent(kq, NULL, 0, &tevent, 1, NULL);
		if (nev == -1)
			throw std::runtime_error("Error: kevent error");
		// 서버로 연결요청 왔을때
		int index = findServer(tevent.ident);
		if (index >= 0)
			acceptClient(kq, index);
		else // 클라이언트로 연결요청이 들어왔을 때 //recv data
		{
			if (tevent.filter == EVFILT_READ)
			{
				char *buffer = new char[tevent.data];
				ssize_t bytesRead = recv(tevent.ident, buffer, tevent.data, 0);
				Request *req = static_cast<Request*>(tevent.udata);
				//----------------------------------------------- testcode
					std::cerr << " ----------------------------------------------- recv " << tevent.ident << std::endl;
					write(1, buffer, tevent.data);
					std::cerr << std::endl;
				//-----------------------------------------------	
				// recvData()
				if (bytesRead == false || req->getConnection() == "close")
				{
					// 맵에 있는 request 주소 삭제 testcode
					std::cerr << "###################### client end ##############################" << std::endl;
					close(req->getSocket());
					delete req;
					_requests.erase(tevent.ident);
				}
				else
				{
		
					try{
						if (req->getState() == request::READY)
							req->parsing(buffer, tevent.data);
						else if (req->getState() == request::POST)
						{
							if (req->getTransferEncoding() != "chunked")
								req->setBuffer(buffer, tevent.data);
						}
						if (req->getTransferEncoding() == "chunked")
						{
							std::cerr << "==================chunked=====================" << std::endl;
							if (req->getBuffer().empty() == false && req->getChunkedBuffer().empty() == true)
								req->parseChunkedData(req, req->getBuffer());
							else
							{
								//updatedBuffer = std::string(buffer, tevent.data);
								req->parseChunkedData(req, std::string(buffer, tevent.data));
								//req->parseChunkedData(req, updatedBuffer);
							}
						}
						if (req->getMethod().size() && req->getBuffer().size() == req->getContentLength())
						{
							// --------------------------------------------------- testcode
							//std::cerr << "req->getBuffer().size():" << req->getBuffer().size() << std::endl;
							//std::cerr << "req->getContentLe().size():" << req->getContentLength() << std::endl;	
							if (req->getMethod() == "POST" && (req->getContentLength() == 0 || req->getBuffer().size() == 0))
								throw 405;
							AResponse* response = selectMethod(req, kq);
							response->createResponse();
							EV_SET(&tevent, tevent.ident, EVFILT_WRITE, EV_ADD, 0, 0, response);
							kevent(kq, &tevent, 1, NULL, 0, NULL);
						}
					} catch(const int errnum) {
						// std::cerr << "errnum : " << errnum << std::endl;
						sendErrorPage(tevent.ident, errnum);
						close(tevent.ident);
					} catch(const std::exception& e) {
						std::cerr << "exception error : " << e.what() << std::endl;
					}
				}
				delete[] buffer;
			}
			else if (tevent.filter == EVFILT_WRITE)
			{	
				sendData(tevent);
			}
		}
	}
}

AResponse* Operation::selectMethod(Request* req, int kq) const
{
	AResponse *result;
	const std::string method = req->getMethod();
	// testcode
	std::cerr << "============method==================" << std::endl;
	std::cerr << method << std::endl;
	
	if (method == "PUT")
	 	result = new Post(req, kq);
	if (method == "GET")
		result = new Get(req, kq);
	if (method == "POST")
		result = new Post(req, kq);
	if (method == "DELETE")
		result = new Delete(req, kq);	
	return result;
}

void Operation::acceptClient(int kq, int index)
{
	int				requestFd;
	sockaddr_in		requestAddr;
	socklen_t		requestLen;
	// struct kevent	revent;
	
	requestFd = accept(_servers[index].getSocket(), reinterpret_cast<struct sockaddr*>(&requestAddr), &requestLen);
	if (requestFd == -1)
		throw std::logic_error("Error: Accept failed");

	Request *request = new Request(requestFd, _servers[index]);
	_requests.insert(std::make_pair(requestFd, request));
	util::setEvent(request, kq, EVFILT_READ);
}


// void Operation::makeResponse(struct kevent *tevent, int kq, Request* req)
// {
// 	//get, head, delete
// 	AResponse* response = new Get(req);
// 	// 응답 헤더
// 	response->createResponse();
// 	// 있을수도있고 없을 수도 있습니다.
// 	response->createResponseMain();

// 	EV_SET(tevent, tevent.ident, EVFILT_WRITE, EV_ADD, 0, 0, response);
// 	// EVFILT_TIMER
// 	kevent(kq, tevent, 1, NULL, 0, NULL);
// }

void Operation::sendData(struct kevent& tevent)
{
	// response주소도 저장해야 하나???
	AResponse* res = static_cast<AResponse*>(tevent.udata);
	res->stamp();
	size_t byteWrite = send(tevent.ident, res->getBuffer().str().c_str(), res->getBuffer().str().length(), 0);
	std::cerr << "==============================response data==============================" << std::endl;
	std::cerr << res->getBuffer().str().c_str() << std::endl;
	//-------------------------------------------------------------  testcode
	// std::cout << res->getBuffer().str() << std::endl;
	/*
		for (size_t i = 0; i < res->getBuffer().str().length(); ++i)
		{
			std::cerr << (int)res->getBuffer().str() << std::end;
		}
	*/
	std::cout << "write byte count :" << byteWrite << std::endl;
	//-------------------------------------------------------------
	delete res;
	close(tevent.ident);
	// send
	// 리스폰스 객체의 데이터를 소켓으로 send하는 부분	
	// send(client_fd, result.c_str(), result.length(), 0);
	// return;
}

// void test_print_event(struct kevent event)
// {
// 	std::cout << "\n===============================================\n";
// 	std::cout << "event ident : " << event.ident << "\n";
// 	std::cout << "event filter : " << event.filter << "\n";
// 	std::cout << "event flags : " << event.flags << "\n";
// 	std::cout << "event fflags : " << event.fflags << "\n";
// 	std::cout << "event data : " << event.data << "\n";
// 	std::cout << "event udata : " << event.udata << "\n";
// 	std::cout << "===============================================\n" << std::endl;
// }
