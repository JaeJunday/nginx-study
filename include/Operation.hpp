#pragma once

#include "Client.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "enum.hpp"
#include "Util.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h> // sockaddr_in
#include <sys/types.h>  // socket, bind
#include <sys/socket.h> // socket
#include <sys/event.h>  // kqueue

class Operation {
private:
	std::map<uint32_t, std::vector<Server> >	_servers;
	std::map<int, Client *>						_clients;
public:
	~Operation();
	void setServer(Server& server);
		void compareServer(Server& server);
		void compareServerName(std::vector<std::string>& strs1, std::vector<std::string>& strs2);
	void start();
		void registerServers(int kq);
			int createBoundSocket(uint32_t listen);
		void handleTriggerEvent(int kq);
			void handleEvent(int kq, const struct kevent* tevents, int nev);
				std::vector<Server>* findServers(uintptr_t ident);
				void acceptClient(int kq, int fd, std::vector<Server>& servers);
					void setSocketOption(int socketFd);
				bool handleReadEvent(const struct kevent& tevent);
				bool handleWriteEvent(const struct kevent& tevent);
				void handleError(const struct kevent& tevent, int errnum);
	void CleanUpClientResources(const struct kevent& tevent, Client* client);
};