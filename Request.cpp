#include "Request.hpp"

Request::Request(int socket)
    : _socket(socket)
{
}

// Request::~Request()
// {
// }

// void Request::checkHeader() const
// { 

// }

// void Request::parsing()
// {
//     try {
        // checkHeader();
        // setOther();
        // makeMain();
//     } catch (std::exception &e) {
    
//     }
// }

int Request::getSocket() const
{
    return _socket;
}

void Request::setBuffer(char* buffer)
{
    _buffer = buffer;
}

const char* Request::getBuffer() const
{
    return _buffer;
}