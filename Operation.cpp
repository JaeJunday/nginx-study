#include "Operation.hpp"

Operation::Operation() {}

Operation::~Operation() 
{
    _servers.clear();
}

void Operation::setServer(const Server& server) 
{
    _servers.push_back(server);
}

const std::vector<Server>& Operation::getServers() const
{
    return _servers;
}

void Operation::start() {
    // 서버 시작 로직을 구현합니다.
    // ...
    Socket sk;
    
    sk.server_fd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sk.server_fd, F_SETFL, O_NONBLOCK);
}
