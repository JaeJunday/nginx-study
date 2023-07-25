#include "Operation.hpp"

void Operation::setServer(const Server& server) 
{
	_servers.push_back(server);
}

const std::vector<Server>& Operation::getServers() const
{
	return _servers;
}

int Operation::createBoundSocket(int port)
{
	int socketFd = socket(AF_INET, SOCK_STREAM, FALLOW);
	if (socketFd == -1)
		throw std::logic_error("Error: Socket creation failed");
	sockaddr_in serverAddr;
	int optval = 1;
	memset((char*)&serverAddr, 0, sizeof(sockaddr_in));

	// ip v4
	serverAddr.sin_family = AF_INET; 

// 0 ~ 255. 255. 255. 255
//       1   1      1   1
// 	char  char  char char 
// 	8         8       8     8 
// 	atoi(255)
// 	4
// 	i = 192 << 24 
// 	i = 168 << 16
// 	i = 0 << 8
// 	i = 1

	// ip address
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	// host port
	serverAddr.sin_port = htons(4242);

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
			std::string portStr = _servers[i].getValue(server::LISTEN);
			double port = strtod(portStr.c_str(), NULL);
			_servers[i].setSocket(createBoundSocket(static_cast<int>(port)));
			fcntl(_servers[i].getSocket(), F_SETFL, O_NONBLOCK);
			if (listen(_servers[i].getSocket(), SOMAXCONN) == -1)
				throw std::logic_error("Error: Listen failed");
		} catch (std::exception &e) {
			std::cout << e.what() << std::endl;
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
			kevent(kq, &revent, 1, NULL, 0, NULL);
			// EV_SET(&revent, requestFd, EVFILT_WRITE, EV_ADD, 0, 0, nullptr);
			// _requests.push_back(request);
		}
		else // 클라이언트로 연결요청이 들어왔을 때
		{
			// test_print_event(tevent);
			// for (ITOR iter = _requests.begin(); iter != _requests.end(); ++iter)
			// {
				// 이벤트 들어온 소켓 번호 찾는 부분
				// if(tevent.ident == static_cast<uintptr_t>(iter->getSocket()))
				// {
					// method == POST 일때만 한번 더 받음
					// GET 이랑 DELETE일때는 한번 더 안받음
					// POST일 때는 리퀘스트 멤버 버퍼를 만들어서 헤더를 저장해 놓음
					// std::cout << "HERE" << std::endl;
					// test_print_event(tevent);
					if (tevent.filter == EVFILT_READ)
					{
						char *buffer = new char[tevent.data];
						ssize_t bytesRead = recv(tevent.ident, buffer, tevent.data, 0);
						Request *req = static_cast<Request*>(tevent.udata);
						std::cout <<  "bytesRead :: " << bytesRead << std::endl;
						if (bytesRead == false)
						{
							close(req->getSocket());
							delete req;
						}
						else if (req->getState() == request::POST)
						{
							req->setMain(buffer, tevent.data);
							std::cout << req->getMain() << std::endl;
						}
						else
						{
							test_print_event(tevent);
							req->parsing(buffer, tevent.data);
						}
						delete[] buffer;
					}
					else
					{	
						std::cout << "send" << std::endl;
					}
				// }
			// }
		}
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


// if (tevent.filter == EVFILT_READ)
			// recv
			// >>>>>
			// else
			// send