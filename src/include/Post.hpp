#pragma once

#include "AResponse.hpp"

class Post : public AResponse
{
    private:
    public:
        Post(Request* request);
	    void createResponseHeader(); // override
};

