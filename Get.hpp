#pragma once

#include "AResponse.hpp"

class Get : public AResponse
{
    private:
    public:
        Get();
        Get(const Get& src);
        virtual ~Get();
        Get& operator=(Get const& rhs);
	    void createResponseHeader(); // override
		void createResponseMain(); // override
};
