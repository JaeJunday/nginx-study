#pragma once

#include "AResponse.hpp"
#include "Server.hpp"

#include <cstdio>

class Delete : public AResponse
{
    private:
    public:
        Delete(Request* request);
		void createResponse(); // override
        void removeFile() const;
        void checkLimitExcept() const;
};

