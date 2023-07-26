#include "Operation.hpp"

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

//-------------------------------------------delete
	std::cout << ipPort[0] << std::endl;
	std::cout << ipPort[1] << std::endl;
//-------------------------------------------

	uint32_t ip = 0x0000000; 
	uint32_t port = 0;
	if (ipPort.size() == 1)
		port = util::stoui(ipPort[0]);
	else if (ipPort.size() == 2)
	{
		ip = util::convertIp(ipPort[0]); 
		port = util::stoui(ipPort[1]);
	}

//-------------------------------------------delete
	std::cout << "ip " << ip << std::endl;
	std::cout << "port " << port << std::endl;
//-------------------------------------------

	// ip v4
	serverAddr.sin_family = AF_INET; 
	// ip address
	serverAddr.sin_addr.s_addr = htonl(ip);
	// host port
	serverAddr.sin_port = htons(port);

	setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (bind(socketFd, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1)
		throw std::logic_error("Error: Socket bind failed");
	return socketFd;
}

void Operation::start() {
	// 서버 시작 로직을 구현합니다.
	// ...
	// for(int i = 0; i < _servers.size(); ++i)
	for(int i = 0; i < 1; ++i)
	{
		try {
			_servers[i].setSocket(createBoundSocket(_servers[i].getValue(server::LISTEN)));
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

	EV_SET(&event, _servers[0].getSocket(), EVFILT_READ, EV_ADD, 0, 0, nullptr);
	kevent(kq, &event, 1, NULL, 0, NULL);
	// loop
	while (true)
	{
		nev = kevent(kq, NULL, 0, &tevent, 1, NULL);
		if (nev == -1)
		{
			//kevent error
			throw std::runtime_error("Error: kevent error");
		}
		// 서버로 연결요청이 들어왔을 때
		if (tevent.ident == static_cast<uintptr_t>(_servers[0].getSocket()))
		{
			int				requestFd;
			sockaddr_in		requestAddr;
			socklen_t		requestLen;
			struct kevent	revent;
			
			requestFd = accept(_servers[0].getSocket(), reinterpret_cast<struct sockaddr*>(&requestAddr), &requestLen);
			if (requestFd == -1)
				throw std::logic_error("Error: Accept failed");

			// 연결요청 들어오면 객체 생성하고 리스트에 이어붙임
			Request *request = new Request(requestFd);
			// event 등록이 들어가야한다.
			EV_SET(&revent, requestFd, EVFILT_READ, EV_ADD, 0, 0, request);
			// EV_SET(&revent, requestFd, EVFILT_WRITE, EV_ADD, 0, 0, request);
			kevent(kq, &revent, 1, NULL, 0, NULL);
			// _requests.push_back(request);
		}
		else // 클라이언트로 연결요청이 들어왔을 때
		{
			// method == POST 일때만 한번 더 받음
			// GET 이랑 DELETE일때는 한번 더 안받음
			// POST일 때는 리퀘스트 멤버 버퍼를 만들어서 헤더를 저장해 놓음
			if (tevent.filter == EVFILT_READ)
			{
				char *buffer = new char[tevent.data];
				ssize_t bytesRead = recv(tevent.ident, buffer, tevent.data, 0);
				Request *req = static_cast<Request*>(tevent.udata);
				// std::cout <<  "bytesRead :: " << bytesRead << std::endl;
				if (bytesRead == false || req->getConnection() == "close")
				{
					close(req->getSocket());
					delete req;
				}
				else if (req->getState() == request::POST)
				{
					req->setMain(buffer, tevent.data);
					std::cout << req->getMain() << std::endl;
					std::cout << "메인문 한번 들어옴" << std::endl;
					// if (req->getTransferEncoding() == "chunked")
					// {}
					// else
					// {}
				}
				else
				{
					test_print_event(tevent);
					req->parsing(buffer, tevent.data);
					std::cout << "클라이언트에서 날라온 값" << std::endl;
					write(1, buffer, tevent.data);
					std::cout << std::endl;
					// makeResponse
					if (req->getState() != request::POST)
					{
						// get, head, delete
						AResponse* response = new Get(req);
						// 응답 헤더
						response->createResponseHeader();
						// 있을수도있고 없을 수도 있습니다.
						response->createResponseMain();
						EV_SET(&tevent, tevent.ident, EVFILT_WRITE, EV_ADD, 0, 0, response);
						kevent(kq, &tevent, 1, NULL, 0, NULL);
					}
				}
				delete[] buffer;
			}
			else if (tevent.filter == EVFILT_WRITE)
			{	
				AResponse* res = static_cast<AResponse*>(tevent.udata);
				std::cout << "send" << std::endl;
				std::cout << res->getBuffer().str() << std::endl;
				ssize_t byteWrite = send(tevent.ident, res->getBuffer().str().c_str(), res->getBuffer().str().length(), 0);
				std::cout << byteWrite << std::endl;
				delete res;
				close(tevent.ident);
				// send
				// 리스폰스 객체의 데이터를 소켓으로 send하는 부분	
				// send(client_fd, result.c_str(), result.length(), 0);
				// return;
			}
			// std::cout << "이벤트 한번" << std::endl;
		}
		// std::cout << "이벤트 --- 한번" << std::endl
	}
}


void test_print_event(struct kevent event)
{
	std::cout << "\n===============================================\n";
	std::cout << "event ident : " << event.ident << "\n";
	std::cout << "event filter : " << event.filter << "\n";
	std::cout << "event flags : " << event.flags << "\n";
	std::cout << "event fflags : " << event.fflags << "\n";
	std::cout << "event data : " << event.data << "\n";
	std::cout << "event udata : " << event.udata << "\n";
	std::cout << "===============================================\n" << std::endl;
}
