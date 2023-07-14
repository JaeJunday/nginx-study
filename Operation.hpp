#pragma once

#include "Server.hpp"
#include <iostream>
#include <string>
#include <vector>

class Operation {
private:
    std::vector<Server> servers;

public:
    Operation();
    Operation(const Operation& other);
    Operation& operator=(const Operation& other);
    ~Operation();

    void setServer(const Server& server);
    // Server& getServer(size_t index);
};
