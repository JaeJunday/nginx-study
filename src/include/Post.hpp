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
};

