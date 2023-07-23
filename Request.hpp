#pragma once

#include <iostream>
#include <string>

class Request
{
    private:
        char buffer[];
        std::string _method;
        std::string _host;
        std::string _contentType;
        std::string _contentLength;
    public:
        void Request::parsing();
        void Request::checkHeader() const
};
