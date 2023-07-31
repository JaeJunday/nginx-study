#pragma once

#include "AResponse.hpp"
#include "Server.hpp"

class Post : public AResponse
{
    private:
    public:
        Post(Request* request);
	    void createResponseHeader(const Server& server); // override
	    void createResponseMain(); // override
};

