#pragma once

#include "AResponse.hpp"
#include "Server.hpp"

#include <cstdio>

class Delete : public AResponse
{
    private:
    public:
        Delete(Request* request, int kq);
		void createResponse(); // override
        void removeFile(std::string file) const;
        void checkLimitExcept() const;
};

