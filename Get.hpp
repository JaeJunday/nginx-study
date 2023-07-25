#pragma once

#include "AResponse.hpp"

class Get : public AResponse
{
    private:
    public:
        Get();
        Get(Request* request);
        virtual ~Get();
        Get& operator=(Get const& rhs);
	    void createResponseHeader(); // override
		void createResponseMain(); // override
};
