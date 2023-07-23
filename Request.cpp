#include "Request.hpp"

Request::Request()
{
}

Request::~Request()
{
}

void Request::checkHeader() const
{ 

}

void Request::parsing()
{
    try {
        checkHeader();
        setOther();
        makeMain();
    } catch (std::exception &e) {
    
    }
}

