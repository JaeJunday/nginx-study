#pragma once

#include <iostream>
#include <string>

class Request
{
    private:
        char *buffer[kq-recv-event-length];
        std::string _method;
        std::string _host;
        std::string _contentType;
        std::string _contentLength;
        Request(const Request& src);
        Request& operator=(Request const& rhs);
    public:
        Request(void);
        virtual ~Request(void);

        void Request::parsing();
        void Request::checkHeader() const
};
