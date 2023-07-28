#pragma once

#include "AResponse.hpp"
#include "Get.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "enum.hpp"
#include "Util.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h> // sockaddr_in
#include <sys/types.h>  // socket, bind
#include <sys/socket.h> // socket
#include <sys/event.h>  // kqueue

#define FALLOW 0

class Operation {
private:
    std::vector<Server> _servers;
    std::map<int, Request *> _requests;
public:
    ~Operation();
    void setServer(const Server& server);
    const std::vector<Server>& getServers() const;
    int createBoundSocket(std::string listen);
    void start();

    void acceptClient(int kq);
    //void makeResponse(struct Kevent *tevent, int kq, Request* req);
    void sendData(struct kevent& tevent);
};

void test_print_event(struct kevent event);
