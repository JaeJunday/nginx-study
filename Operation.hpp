#pragma once

#include "Server.hpp"
#include "Socket.hpp"
#include "Request.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <unistd.h>
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
