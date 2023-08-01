#pragma once

#include "AResponse.hpp"
#include "Server.hpp"

class Delete : public AResponse
{
    private:
    public:
        Delete(Request* request);
		void createResponseHeader(std::vector<Server> servers); // override
	    void createResponseMain(); // override
};

