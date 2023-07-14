#include "Operation.hpp"

Operation::Operation() {}

Operation::Operation(const Operation& other) 
{
    servers = other.servers;
}

Operation& Operation::operator=(const Operation& other) 
{
    if (this != &other) {
        servers = other.servers;
    }
    return *this;
}

Operation::~Operation() 
{
    servers.clear();
}

void Operation::setServer(const Server& server) 
{
    servers.push_back(server);
}

void Operation::start() {
    // 서버 시작 로직을 구현합니다.
    // ...
}
