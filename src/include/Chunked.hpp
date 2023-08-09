#pragma once

#include "Util.hpp"
#include "AResponse.hpp"
#include "Server.hpp"
#include "Operation.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <sys/event.h>  // kqueue
#include <algorithm> // min

class Chunked : public AResponse
{
    public:
        Chunked(Request* request, int kq);
		void createResponse(); // override
        void childProcess(int *writeFd, int *readFd);
		void uploadFile(int fd, int kq);
        const std::string printResult(int fd, int kq);
};

