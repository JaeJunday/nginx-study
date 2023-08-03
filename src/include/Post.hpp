#pragma once

#include "AResponse.hpp"
#include "Server.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

class Post : public AResponse
{
    private:
    public:
        Post(Request* request);
		void createResponse(); // override
};

