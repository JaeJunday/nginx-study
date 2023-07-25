#include "AResponse.hpp"
#include "Request.hpp"

AResponse::AResponse()
{
    /* Constructor Implementation */
}

AResponse::AResponse(Request* request) : _request(request)
{
    /* Constructor Implementation */
}

AResponse::AResponse(const AResponse& src)
{
    /* Copy Constructor Implementation */
}

AResponse::~AResponse()
{
    /* Destructor Implementation */
}

AResponse& AResponse::operator=(AResponse const& rhs)
{
    if (this != &rhs)
    {
        /* Assignment Operator Implementation */
    }
    return *this;
}

const std::string AResponse::getData() const
{
    std::time_t now = std::time(NULL);
    char timeStamp[TIME_SIZE];
    std::strftime(timeStamp, sizeof(timeStamp), "Date: %a, %d %b %Y %H:%M:%S GMT", std::localtime(&now));
    return (timeStamp);
}

// 15:53:14 : 0.0.0.0 GET HTTP/1.1 200 Ok ./public 2259

void AResponse::stamp() const
{
    std::cout << getData() << " : "<< _request->getIp() << " " << _request->getMethod() << " " << _request->getVersion() << " "<< _stateCode << " " << _reasonPhrase;
    // std::cout <<  << std::endl;
}