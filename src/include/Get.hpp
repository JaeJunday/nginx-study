#pragma once

#include "AResponse.hpp"

struct PostData;

class Get : public AResponse
{
    private:
    public:
        Get();
        Get(Request* request);
        virtual ~Get();
        Get& operator=(Get const& rhs);
        void createResponseHeader(); //override
		void createResponseMain(); //override
        std::string findLocationPath() const;
        void openPath(const std::string& path) const;
};
