#pragma once

#include "Server.hpp"
#include "Socket.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/event.h>  // kqueue

#define FALLOW 0

class Operation {
private:
    std::vector<Server> _servers;
public:
    void setServer(const Server& server);
    const std::vector<Server>& getServers() const;
    int createBoundSocket(int port);
    void start();
    void timeStamp() const;
};
