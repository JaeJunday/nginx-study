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
		void createResponseHeader(const Server& server); //override;
		void createResponseMain(); // override
        const std::string& findLocationPath(const Server& server) const;
};
