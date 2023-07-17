#include "Operation.hpp"

Operation::Operation() {}

Operation::Operation(const Operation& other) 
{
    _servers = other._servers;
}

Operation& Operation::operator=(const Operation& other) 
{
    if (this != &other) {
        _servers = other._servers;
    }
    return *this;
}

Operation::~Operation() 
{
    _servers.clear();
}

void Operation::setServer(const Server& server) 
{
    _servers.push_back(server);
}

void Operation::start() {
    // 서버 시작 로직을 구현합니다.
    // ...
}
