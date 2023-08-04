#pragma once

#include "AResponse.hpp"
#include "Server.hpp"

class Delete : public AResponse
{
    private:
    public:
        Delete(Request* request, int kq);
		void createResponse(); // override
        void removeFile() const;
};

