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
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
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
	// while (true) --- 웹에서 두 번 클릭해야 넘어감. - kyeonkim
	for (int j = 0; j < 10; ++j)
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
			
			requestFd = accept(_servers[0].getSocket(), reinterpret_cast<struct sockaddr *>(&requestAddr), &requestLen);
			if (requestFd == -1)
				throw std::logic_error("Error: Accept failed");

			// 연결요청 들어오면 객체 생성하고 리스트에 이어붙임
			Request request(requestFd);
			// event 등록이 들어가야한다.
			EV_SET(&revent, requestFd, EVFILT_READ, EV_ADD, 0, 0, nullptr);
			kevent(kq, &revent, 1, NULL, 0, NULL);
			_requests.push_back(request);
		}
		else // 클라이언트로 연결요청이 들어왔을 때
		{
			for (ITOR iter = _requests.begin(); iter != _requests.end(); ++iter)
			{ 
				if(tevent.ident == static_cast<uintptr_t>(iter->getSocket()))
				{   
					char *buf = new char[tevent.data];
					ssize_t bytesRead = recv(iter->getSocket(), buf, tevent.data, 0);
					if (bytesRead == false)
					{
						close(iter->getSocket());
						return;
					}
					iter->setBuffer(buf);
					write(1, buf, tevent.data);
				}
			}
		}
	}
}

// file input ###########################3
	// std::ofstream ff;
	// ff.open("test.txt", std::ofstream::out);
	// for (int i = 0; i < tevent.data; ++i) {
	// 	ff.put(buf[i]);
	// }
	// ff.close();
// file input ###########################3