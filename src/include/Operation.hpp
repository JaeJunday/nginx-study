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

#define FALLOW 0
/*
kyeonkim
[현재 Operation _servers 데이터 구조] - port는 conf의 server 블록 안에 있는 listen
==============================================
			port1		, port2 , ...
			|			  |
			-server		  -server
			-server		  -server
			-...		  -...
==============================================
*/
class Operation {
private:
	std::map<uint32_t, std::vector<Server> >	_servers;
	std::map<int, Client *>						_clients;
public:
	~Operation();
	void setServer(Server& server);
	int createBoundSocket(uint32_t listen);
	std::vector<Server>& findServers(uintptr_t ident);
	void start();

	void acceptClient(int kq, int fd, std::vector<Server>& servers);
	void sendData(struct kevent& tevent);
	void testPipe(std::string buffer);
	Client* selectMethod(Request* req, int kq) const;
	// compare
	void compareServer(std::vector<Server>& servers, Server& server);
	void compareServerName(std::vector<std::string>& strs1, std::vector<std::string>& strs2);

	void processEvent(int kq, struct kevent *tevents, int nev);
};

// remove testcase function
void test_print_event(struct kevent event);


