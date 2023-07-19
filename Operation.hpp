#pragma once

#include "Server.hpp"
#include "Socket.hpp"
#include <iostream>
#include <string>
#include <vector>

class Operation {
private:
    std::vector<Server> _servers;

    Operation(const Operation& other);
    Operation& operator=(const Operation& other);
public:
    Operation();
    ~Operation();

    void setServer(const Server& server);
    const std::vector<Server>& getServers() const;
    void start();
};
