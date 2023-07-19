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
    
	int optval = 1;
    std::string port = _servers[0].getValue(server::LISTEN);
    // if (port.empty())
    //     throw 500 errorpage;
 
    sk.server_fd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sk.server_fd, F_SETFL, O_NONBLOCK);
	setsockopt(sk.server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    sk.server_addr.sin_family = AF_INET;
    sk.server_addr.sin_addr.s_addr = INADDR_ANY;

    double a = strtod(port.c_str(), NULL);
    int tmp = static_cast<int>(a);
    sk.server_addr.sin_port = htons(tmp);
}
