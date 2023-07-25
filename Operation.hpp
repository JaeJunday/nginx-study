#pragma once

#include "AResponse.hpp"
#include "Get.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "enum.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h> // sockaddr_in
#include <sys/types.h>  // socket, bind
#include <sys/socket.h> // socket
#include <sys/event.h>  // kqueue

#define FALLOW 0

typedef std::list<Request>::iterator ITOR;

class Operation {
private:
    std::vector<Server> _servers;
    std::list<Request> _requests;
public:
    void setServer(const Server& server);
    const std::vector<Server>& getServers() const;
    int createBoundSocket(int port);
    void start();
};

void test_print_event(struct kevent event);
