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
        void createResponse(); //override
        // std::string findLocationPath() const;
        void openPath(const std::string& path);
		void fileProcess(const std::string& filePath);
};
