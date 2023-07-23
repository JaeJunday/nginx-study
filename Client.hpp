#pragma once

class Client
{
    private:
    public:
        Client();
        Client(const Client& src);
        virtual ~Client();
        Client& operator=(Client const& rhs);
};
