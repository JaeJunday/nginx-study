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

void Operation::compareServerName(std::vector<std::string>& strs1, std::vector<std::string>& strs2)
{
	for (size_t i = 0; i < strs1.size(); ++i)
	{
		for (size_t j = 0; j < strs2.size(); ++j)
		{
			if (strs1[i] == strs2[j])
				throw std::runtime_error("Error: duplication Server");
		}
	}
}

void Operation::compareServer(std::vector<Server>& servers, Server& server)
{
	if (_servers[server.getListen()].size() > 0)
	{
		std::vector<Server>& servers = _servers[server.getListen()];
		for (size_t i = 0; i < servers.size(); ++i)
			compareServerName(servers[i].getServerName(), server.getServerName());
	}
}

void Operation::setServer(Server& server) 
{
	compareServer(_servers[server.getListen()], server); // set 전에 동일한 서버가 있는지 검사 - kyeonkim
	_servers[server.getListen()].push_back(server);
}
/*
	[Done] - kyeonkim
	- conf 에서 listen 이 같고 server_name 이 다를 경우 다음과 같이 처리해야한다.
	- server socket 에 bind 하기 전에 port가 같은 서버끼리 묶어둔다.
	- 클라이언트에서 요청이 들어올 때 묶은 변수를 한번 탐색해서 해당 서버 이름이 있는지 비교한다.
	- 있으면 처리하고 없으면 서버들 중에 맨 위에 있는([0]) 서버의 이름으로 요청을 처리한다.
*/
/*	kyeonkim
	@des 각 port 해당하는 서버들은 같은 fd 를 가진다. 그러므로 port의 개수만큼 fd가 만들어진다.
*/
int Operation::createBoundSocket(uint32_t port)
{
	int socketFd = socket(AF_INET, SOCK_STREAM, FALLOW);
	if (socketFd == -1)
		throw std::logic_error("Error: Socket creation failed");
	sockaddr_in serverAddr;
	int optval = 1;
	memset((char*)&serverAddr, 0, sizeof(sockaddr_in));
	std::cerr << "http://0.0.0.0" << ":" << port << std::endl; // print server ip:port
	serverAddr.sin_family = AF_INET; 
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(port);
	setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (bind(socketFd, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1)
		throw std::logic_error("Error: Socket bind failed");
	return socketFd;
}
/*	kyeonkim
	@des 하나의 포트에 여러 개의 서버가 있는데 어떤 포트에 있는 서버인지 찾아서 여러 개의 서버 vector 을 넘김
	@return std::vector<Server>
*/
std::vector<Server>& Operation::findServers(uintptr_t ident)
{
	std::map<uint32_t, std::vector<Server> >::iterator it;

 	for (it = _servers.begin(); it != _servers.end(); ++it)
	{
		std::vector<Server>& serverList = it->second; // 맵의 값 (Server 벡터)
		Server& server = serverList[0];
		if (static_cast<uintptr_t>(server.getSocket()) == ident)
			return serverList;
	}
	std::vector<Server> empty;
	return empty;
}

/*	kyeonkim
	@des 이벤트가 동시에 날라올 때, 해당 이벤트를 1개씩 받는게 아니라 다 받고 저장한 뒤에 하나씩 처리한다.
	이벤트를 하나씩 받으면서 하나씩 처리할 경우, 이벤트가 날라온 순서대로 처리되지 않을 가능성이 존재한다.
	이를 방지하기 위해 동시에 날라오는 이벤트를 MAX_EVENT 만큼 받아준다.
	(이벤트 처리를 제외한 모든 로직들은 함수로 뺴야한다. - 리펙토링)
*/
void Operation::processEvent(int kq, struct kevent *tevents, int nev)
{
	for (int i = 0; i < nev; ++i)
	{
		if (tevents[i].udata == NULL) // server block
		{
			std::vector<Server>& servers = findServers(tevents[i].ident);
			if (!servers.empty())
				acceptClient(kq, tevents[i].ident, servers);
		}
		else // client block
		{
			try
			{
				Client* client = reinterpret_cast<Client*>(tevents[i].udata);
				// std::cerr << B_BG_RED << "testcode " << client->getSocket() << RESET << std::endl;
				// std::cerr << B_BG_RED << "testcode " << client << RESET << std::endl;
				if (tevents[i].filter == EVFILT_READ)
				{

					// std::cerr << RED << "recv : " << tevents[i].ident << ":"<< RESET << std::endl;
					// write(1, buffer, bytesRead);
					// std::cerr << std::endl;

					if (tevents[i].ident == client->getSocket())
					{
						client->resetTimerEvent(); // READ 이벤트가 소켓으로 날라올 경우 해당 fd의 타이머 이벤트를 리셋 - kyeonkim
						char* buffer = new char[tevents[i].data];
						/*
							[Feat] - kyeonkim
							- read/recv/write/send 의 반환값인 0, -1 을 모두 처리해야한다.
							- errno 쓰지말라
						*/
						ssize_t bytesRead = recv(tevents[i].ident, buffer, tevents[i].data, 0);
						// std::cerr << B_BLUE << "testcode fd :"<< client->getSocket() << "access client" << RESET << std::endl;
						if (bytesRead == false || client->getReq().getConnection() == "close")
						{
							std::cerr << B_RED << "testcode fd :"<< client->getSocket() << " close client" << RESET << std::endl;
							client->clearClient();
							close(tevents[i].ident);
							_clients.erase(tevents[i].ident);
							delete client; // 소멸자 부를 때 request 제거
						}
						else
						{
							client->handleRequest(&tevents[i], buffer);
						}
						delete[] buffer;
					}
					else if(tevents[i].ident == client->getReadFd())
					{
						// std::cerr << YELLOW << "readfd" << RESET << std::endl;
						client->printResult(static_cast<size_t>(tevents[i].data));
					}	
				}
				else if (tevents[i].filter == EVFILT_WRITE)
				{
					if (tevents[i].ident == client->getSocket())
					{
						client->resetTimerEvent(); // WRITE 이벤트가 소켓으로 날라올 경우 해당 fd의 타이머 이벤트를 리셋 - kyeonkim
						if (client->sendData(tevents[i]) == false)
							_clients.erase(tevents[i].ident);
					}
					else if (tevents[i].ident == client->getWriteFd())
					{
						// std::cerr << YELLOW << "writefd" << RESET << std::endl;
						client->uploadFile(tevents[i].data);
					} 
				}
				else if (tevents[i].filter == EVFILT_PROC)
				{
					if (tevents[i].fflags & NOTE_EXIT)
					{
						std::cerr << B_RED << "testcode " << "EVFILT_PROC" << "fd: " << client->getSocket() << RESET << std::endl;
						client->endChildProcess();
					}
				}
				else if (tevents[i].filter == EVFILT_TIMER) // 타이머 완료 시 클라이언트 제거 - kyeonkim
				{
					std::cerr << B_BG_YELLOW << "testcode FILTER == TIMER EVENT > DELETE CLIENT" << RESET << std::endl;
					client->clearClient();
					close(tevents[i].ident);
					_clients.erase(tevents[i].ident);
					delete client;
				}
			}
			catch (const int errnum)
			{	
				Client* client = static_cast<Client*>(tevents[i].udata);
			std::cerr << RED <<  "fd: " << client->getSocket() <<  "in trycatch error delete read event" << RESET << std::endl;
				client->deleteReadEvent();
				client->errorProcess(errnum);
				client->addEvent(tevents[i].ident, EVFILT_WRITE);
				client->getReq().setEventState(EVFILT_WRITE);
			}
			catch(const std::exception& e)
			{
				std::cerr << "exception error : " << e.what() << std::endl;
			}
		}
	}
}

// Server Start
void Operation::start() {
	int kq;
	int nev;
	struct kevent event;
	struct kevent tevents[event::MAX_EVENT];	 /* Event triggered */
	std::map<uint32_t, std::vector<Server> >::iterator it;

	kq = kqueue();
    for (it = _servers.begin(); it != _servers.end(); ++it)
	{
        uint32_t port = it->first; // 맵의 키
		try {
			int socketFd = createBoundSocket(port);
        	std::vector<Server>& serverList = it->second; // 맵의 값 (Server 벡터)
        	std::vector<Server>::iterator serverIt;
			/*	kyeonkim
				@des 각 port에 해당하는 서버들은 같은 fd를 사용하므로 해당 port에 속하는 서버들을 돌며
				fd 값을 setting 해준다.
			*/
			for (serverIt = serverList.begin(); serverIt != serverList.end(); ++serverIt)
			{
				Server& server = *serverIt;
				server.setSocket(socketFd);
				fcntl(server.getSocket(), F_SETFL, O_NONBLOCK);
				if (listen(server.getSocket(), 1024) == -1) // SOMAXCONN == 128
					throw std::logic_error("Error: Listen failed");
				// 각 서버에 READ Event 를 검 - kyeonkim
				EV_SET(&event, server.getSocket(), EVFILT_READ, EV_ADD, 0, 0, NULL);
				kevent(kq, &event, 1, NULL, 0, NULL);
			}
		} catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
			continue;
		}
	}
	/*
		[Done] - kyeonkim
		- 평가표에 The select() (or equivalent) should be in the main loop and should check file descriptors for read and write AT THE SAME TIME.
		  란 항목이 있는데 항목의 처리는 다음과 같다.
		- conf 에 max_event 라는 키워드가 있으면 해당 키워드 value 값으로 이벤트 공간을 설정해야한다. > 굳이 conf에 해당 키워드를 넣을 필요는 없다. 그냥 define 1024 해서 사용
		- 이벤트 공간을 설정했으면 nev = kevent(kq, NULL, 0, &tevent, 1, NULL); 이렇게 1개씩 받는 것이 아니라
		설정한 공간만큼 받는다.
		- 그러면 nev 변수에 받은 이벤트 수가 들어오게되고 해당 nev를 loop 시켜서 events[nev].filter 이런식으로 이벤트를 처리해야한다.
	*/
	while (true)
	{
		nev = kevent(kq, NULL, 0, tevents, event::MAX_EVENT, NULL);
		if (nev == -1)
			throw std::runtime_error("Error: kevent error");
		processEvent(kq, tevents, nev); // 이벤트 loop 처리 - kyeonkim
	}
}

void Operation::acceptClient(int kq, int fd, std::vector<Server>& servers)
{
	int				socketFd;
	sockaddr_in		socketAddr;
	socklen_t		socketLen;
	
std::cerr << GREEN << "testcode" << "================ ACCEPT =========================" << RESET << std::endl;
	socketFd = accept(fd, reinterpret_cast<struct sockaddr*>(&socketAddr), &socketLen);
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
	Request *request = new Request(servers); // server vector 가 들어가야한다. - kyeonkim
	Client* client = new Client(request, kq, socketFd);
	_clients.insert(std::make_pair(socketFd, client));
	client->addEvent(socketFd, EVFILT_READ);
	client->addEvent(socketFd, EVFILT_TIMER); // 타이머 이벤트 추가 - kyeonkim
	client->getReq().setEventState(EVFILT_READ);
}
