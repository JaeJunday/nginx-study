#pragma once

#include "Server.hpp"
#include <iostream>
#include <string>
#include <vector>

class Operation {
private:
public:
    std::vector<Server> _servers;

    Operation();
    Operation(const Operation& other);
    Operation& operator=(const Operation& other);
    ~Operation();

    void setServer(const Server& server);
    // Server& getServer(size_t index);
    void start();
};
