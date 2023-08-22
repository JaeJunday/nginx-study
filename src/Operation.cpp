#include "Operation.hpp"

Operation::~Operation()
{
	std::map<int, Client*>::iterator it;
	if (!_clients.empty())
	{
		for (it = _clients.begin(); it != _clients.end(); ++it)
			delete it->second; // 소멸자 호출됨
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
	for (size_t i = 0; i < _servers.size(); ++i)
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
			if (listen(_servers[i].getSocket(), 1024) == -1) // SOMAXCONN == 128
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
		EV_SET(&event, _servers[i].getSocket(), EVFILT_READ, EV_ADD, 0, 0, NULL);
		kevent(kq, &event, 1, NULL, 0, NULL);
	}

	while (true)
	{
		nev = kevent(kq, NULL, 0, &tevent, 1, NULL); // EVFILT_READ, EVFILT_WRITE 이벤트가 감지되면 이벤트 감지 개수를 반환
		if (nev == -1)
			throw std::runtime_error("Error: kevent error");
		int serverIndex = findServer(tevent.ident);
		if (serverIndex >= 0) // 서버일 경우
		{
			acceptClient(kq, serverIndex);
		}
		else // 클라이언트일 경우
		{
			try
			{
				Client* client = reinterpret_cast<Client*>(tevent.udata);
				// std::cerr << B_BG_RED << "testcode " << client->getSocket() << RESET << std::endl;
				// std::cerr << B_BG_RED << "testcode " << client << RESET << std::endl;
				if (tevent.filter == EVFILT_READ)
				{

					// std::cerr << RED << "recv : " << tevent.ident << ":"<< RESET << std::endl;
					// write(1, buffer, bytesRead);
					// std::cerr << std::endl;

					if (tevent.ident == client->getSocket())
					{
						client->resetTimerEvent(); // READ 이벤트가 소켓으로 날라올 경우 해당 fd의 타이머 이벤트를 리셋 - kyeonkim
						char* buffer = new char[tevent.data];
						ssize_t bytesRead = recv(tevent.ident, buffer, tevent.data, 0);
						// std::cerr << B_BLUE << "testcode fd :"<< client->getSocket() << "access client" << RESET << std::endl;
						if (bytesRead == false || client->getReq().getConnection() == "close")
						{
							std::cerr << B_RED << "testcode fd :"<< client->getSocket() << " close client" << RESET << std::endl;
							client->clearClient();
							close(tevent.ident);
							_clients.erase(tevent.ident);
							delete client; // 소멸자 부를 때 request 제거
						}
						else
						{
							client->handleRequest(&tevent, buffer);
						}
						delete[] buffer;
					}
					else if(tevent.ident == client->getReadFd())
					{
						// std::cerr << YELLOW << "readfd" << RESET << std::endl;
						client->printResult(static_cast<size_t>(tevent.data));
					}	
				}
				else if (tevent.filter == EVFILT_WRITE)
				{
					if (tevent.ident == client->getSocket())
					{
						client->resetTimerEvent(); // WRITE 이벤트가 소켓으로 날라올 경우 해당 fd의 타이머 이벤트를 리셋 - kyeonkim
						if (client->sendData(tevent) == false)
							_clients.erase(tevent.ident);
					}
					else if (tevent.ident == client->getWriteFd())
					{
						// std::cerr << YELLOW << "writefd" << RESET << std::endl;
						client->uploadFile(tevent.data);
					} 
				}
				else if (tevent.filter == EVFILT_PROC)
				{
					if (tevent.fflags & NOTE_EXIT)
					{
						std::cerr << B_RED << "testcode " << "EVFILT_PROC" << "fd: " << client->getSocket() << RESET << std::endl;
						client->endChildProcess();
					}
				}
				else if (tevent.filter == EVFILT_TIMER) // 타이머 완료 시 클라이언트 제거 - kyeonkim
				{
					std::cerr << B_BG_YELLOW << "testcode FILTER == TIMER EVENT > DELETE CLIENT" << RESET << std::endl;
					client->clearClient();
					close(tevent.ident);
					_clients.erase(tevent.ident);
					delete client;
				}
			}
			catch (const int errnum)
			{	
				Client* client = static_cast<Client*>(tevent.udata);
			std::cerr << RED <<  "fd: " << client->getSocket() <<  "in trycatch error delete read event" << RESET << std::endl;
				client->deleteReadEvent();
				client->errorProcess(errnum);
				client->addEvent(tevent.ident, EVFILT_WRITE);
				client->getReq().setEventState(EVFILT_WRITE);
			}
			catch(const std::exception& e)
			{
				std::cerr << "exception error : " << e.what() << std::endl;
			}
		}
	}
}

void Operation::acceptClient(int kq, int index)
{
	int				socketFd;
	sockaddr_in		socketAddr;
	socklen_t		socketLen;
	
std::cerr << GREEN << "testcode" << "================ ACCEPT =========================" << RESET << std::endl;
	socketFd = accept(_servers[index].getSocket(), reinterpret_cast<struct sockaddr*>(&socketAddr), &socketLen);
	if (socketFd == -1)
		throw std::runtime_error("Error: Accept failed");
	struct linger linger_opt;
    linger_opt.l_onoff = 1; // Linger 활성화
    linger_opt.l_linger = 0; // Linger 시간 (0은 즉시 소켓 버퍼를 비우도록 설정)
    setsockopt(socketFd, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(linger_opt));
	int socket_option = 1;
	setsockopt(socketFd, SOL_SOCKET, SO_NOSIGPIPE, &socket_option, sizeof(socket_option));
// std::cerr << YELLOW << "socketFd: " << socketFd <<  RESET << std::endl;
	fcntl(socketFd, F_SETFL, O_NONBLOCK);
	Request *request = new Request(_servers[index]);
	Client* client = new Client(request, kq, socketFd);
	_clients.insert(std::make_pair(socketFd, client));
	client->addEvent(socketFd, EVFILT_READ);
	client->addEvent(socketFd, EVFILT_TIMER); // 타이머 이벤트 추가 - kyeonkim
	client->getReq().setEventState(EVFILT_READ);
}
