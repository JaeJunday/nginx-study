#pragma once

#include "AResponse.hpp"

struct PostData;

class Get : public AResponse
{
    public:
        Get();
        Get(Request* request);
        virtual ~Get();
        Get& operator=(Get const& rhs);
        void createResponse(); //override
        // std::string findLocationPath() const;
        void openPath(const std::string& path);
        void fileProcess(const std::string& filePath, std::stringstream& body);
        void pushBuffer(std::stringstream& body);
		void autoIndexProcess(DIR* dirStream, std::stringstream& body);
};
