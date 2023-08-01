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
        void createResponseHeader(std::vector<Server> servers); //override
		void createResponseMain(); //override
        std::string findLocationPath(std::vector<Server> servers) const;
        std::string findFilename(const std::string& filePath) const;
};
