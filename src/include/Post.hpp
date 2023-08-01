#pragma once

#include "AResponse.hpp"
#include "Server.hpp"

class Post : public AResponse
{
    private:
    public:
        Post(Request* request);
		void createResponseHeader(std::vector<Server> servers); // override
	    void createResponseMain(); // override
};

