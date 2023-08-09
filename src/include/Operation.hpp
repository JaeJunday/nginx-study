#pragma once

#include "AResponse.hpp"
#include "Get.hpp"
#include "Post.hpp"
#include "Delete.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "enum.hpp"
#include "Util.hpp"
#include "Error.hpp"

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
    int findServer(uintptr_t ident) const;
    void start();

    void acceptClient(int kq, int index);
    //void makeResponse(struct Kevent *tevent, int kq, Request* req);
    void sendData(struct kevent& tevent);
    void testPipe(std::string buffer);
    
    void handleResponse(Request* req, int kq, struct kevent *tevent, char* buffer);
    
    AResponse* selectMethod(Request* req, int kq) const;
};

// remove testcase function
void test_print_event(struct kevent event);
