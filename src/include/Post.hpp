#pragma once

#include "AResponse.hpp"
#include "Server.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <sys/event.h>  // kqueue
#include <algorithm> // min

class Post : public AResponse
{
    private:
    public:
        Post(Request* request);
		void createResponse(); // override
        void childProcess(int *writeFd, int *readFd);
		void uploadFile(int fd, int kq);
        const std::string& printResult(int fd, int kq);
};

