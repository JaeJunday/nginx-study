#include "Operation.hpp"

Operation::~Operation()
{
	std::map<int, Client*>::iterator it;

	if (!_clients.empty())
		for (it = _clients.begin(); it != _clients.end(); ++it)
			delete it->second;
}

void Operation::setServer(Server& server) 
{
	compareServer(server);
	_servers[server.getListen()].push_back(server);
}

void Operation::compareServer(Server& server)
{
	if (_servers[server.getListen()].size() > 0)
	{
		std::vector<Server>& servers = _servers[server.getListen()];
		for (size_t i = 0; i < servers.size(); ++i)
			compareServerName(servers[i].getServerName(), server.getServerName());
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

void Operation::start()
{
	int kq = kqueue();

	registerServers(kq);
	handleTriggerEvent(kq);
}

void Operation::registerServers(int kq)
{
	struct kevent event;
	std::map<uint32_t, std::vector<Server> >::iterator it;
	
    for (it = _servers.begin(); it != _servers.end(); ++it)
	{
        uint32_t port = it->first;
		try 
		{
			int socketFd = createBoundSocket(port);
        	std::vector<Server>& serverList = it->second;
        	std::vector<Server>::iterator serverIt;
			for (serverIt = serverList.begin(); serverIt != serverList.end(); ++serverIt)
			{
				Server& server = *serverIt;
				server.setSocket(socketFd);
				fcntl(server.getSocket(), F_SETFL, O_NONBLOCK);
				if (listen(server.getSocket(), MAX_LISTEN) == -1)
					throw std::logic_error("Error: Listen failed");
				EV_SET(&event, server.getSocket(), EVFILT_READ, EV_ADD, 0, 0, NULL);
				kevent(kq, &event, 1, NULL, 0, NULL);
			}
		}
		catch (std::exception &e) 
		{
			std::cerr << e.what() << std::endl;
		}
	}
}

int Operation::createBoundSocket(uint32_t port)
{
	int socketFd = socket(AF_INET, SOCK_STREAM, SOCKET_TYPE);
	if (socketFd == -1)
		throw std::logic_error("Error: Socket creation failed");
	sockaddr_in serverAddr;
	int optval = 1;
	memset((char*)&serverAddr, 0, sizeof(sockaddr_in));
	std::cerr << "http://0.0.0.0" << ":" << port << std::endl;
	serverAddr.sin_family = AF_INET; 
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(port);
	setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (bind(socketFd, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1)
		throw std::logic_error("Error: Socket bind failed");
	return socketFd;
}

void Operation::handleTriggerEvent(int kq)
{
	int nev;
	struct kevent tevents[MAX_EVENT];

	while (true)
	{
		nev = kevent(kq, NULL, 0, tevents, MAX_EVENT, NULL);
		if (nev == -1)
			throw std::runtime_error("Error: kevent error");
		handleEvent(kq, tevents, nev);
	}
}

void Operation::handleEvent(int kq, const struct kevent *tevents, int nev)
{
	for (int i = 0; i < nev; ++i)
	{
		if (tevents[i].udata == NULL)
		{
			std::vector<Server>* servers = findServers(tevents[i].ident);
			if (servers != NULL)
				acceptClient(kq, tevents[i].ident, *servers);
		}
		else
		{
			try
			{
				Client* client = reinterpret_cast<Client*>(tevents[i].udata);
				switch (tevents[i].filter) 
				{
					case EVFILT_READ:
						if (handleReadEvent(tevents[i]) == false)
							return;	
						break;
					case EVFILT_WRITE:
						if (handleWriteEvent(tevents[i]) == false)
							return;
						break;
					case EVFILT_PROC:
						client->handleEndProcess();
						break;
					case EVFILT_TIMER:
						CleanUpClientResources(tevents[i], client);
						return;
				}
			}
			catch (const int errnum) 
			{
				handleError(tevents[i], errnum);
			}
			catch(const std::exception& e)
			{
				std::cerr << "exception error : " << e.what() << std::endl;
			}
		}
	}
}

std::vector<Server>* Operation::findServers(uintptr_t ident)
{
	std::map<uint32_t, std::vector<Server> >::iterator it;

 	for (it = _servers.begin(); it != _servers.end(); ++it)
	{
		std::vector<Server>* serverList = &(it->second);
		Server& server = (*serverList)[0];
		if (static_cast<uintptr_t>(server.getSocket()) == ident)
			return serverList;
	}
	return NULL;
}

void Operation::acceptClient(int kq, int fd, std::vector<Server>& servers)
{
	int				socketFd;
	sockaddr_in		socketAddr;
	socklen_t		socketLen;
	
	socketFd = accept(fd, reinterpret_cast<struct sockaddr*>(&socketAddr), &socketLen);
	if (socketFd == -1)
		throw std::runtime_error("Error: Accept failed");
	setSocketOption(socketFd);
	Request *request = new Request(servers);
	Client* client = new Client(request, kq, socketFd);
	_clients.insert(std::make_pair(socketFd, client));
}

void Operation::setSocketOption(int socketFd)
{
	struct linger linger_opt;
	int socket_option = 1;

    linger_opt.l_onoff = 1;
    linger_opt.l_linger = 0;
    setsockopt(socketFd, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(linger_opt));
	setsockopt(socketFd, SOL_SOCKET, SO_NOSIGPIPE, &socket_option, sizeof(socket_option));
	fcntl(socketFd, F_SETFL, O_NONBLOCK);
}

bool Operation::handleReadEvent(const struct kevent &tevent)
{
	Client* client = reinterpret_cast<Client*>(tevent.udata);

	if (tevent.ident == static_cast<uintptr_t>(client->getSocket()))
	{
		client->resetTimerEvent();
		char* buffer = new char[tevent.data];
		ssize_t bytesRead = recv(tevent.ident, buffer, tevent.data, 0);
		if (bytesRead > 0)
			client->getReq().handleRequest(tevent, buffer);
		else if (bytesRead <= 0 || client->getReq().getConnection() == "close")
		{
			CleanUpClientResources(tevent, client);
			delete[] buffer;
			return false;
		}
	}
	else if(tevent.ident == static_cast<uintptr_t>(client->getReadFd()))
		client->readPipe(static_cast<size_t>(tevent.data));
	return true;
}

bool Operation::handleWriteEvent(const struct kevent& tevent)
{
	Client* client = reinterpret_cast<Client*>(tevent.udata);
	
	if (tevent.ident == static_cast<uintptr_t>(client->getSocket()))
	{
		client->resetTimerEvent();
		if (client->sendData(tevent) == false)
		{
			CleanUpClientResources(tevent, client);
			return false;
		}
	}
	else if (tevent.ident == static_cast<uintptr_t>(client->getWriteFd()))
		client->writePipe(tevent.data);
	return true;
}

void Operation::handleError(const struct kevent& tevent, int errnum)
{
	Client* client = static_cast<Client*>(tevent.udata);
	
	client->deleteSocketReadEvent();
	client->handleError(errnum);
	client->addSocketWriteEvent();
}

void Operation::CleanUpClientResources(const struct kevent& tevent, Client* client)
{
	client->clearClient();
	close(tevent.ident);
	_clients.erase(tevent.ident);
	delete client;
}
