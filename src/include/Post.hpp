#pragma once

#include "AResponse.hpp"
#include "Server.hpp"

class Post : public AResponse
{
    private:
    public:
        Post(Request* request);
		void createResponse(); // override
};

