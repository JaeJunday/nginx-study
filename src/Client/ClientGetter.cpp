#include "Client.hpp"

int Client::getWriteFd() const
{
	return _writeFd[1];
}

int Client::getReadFd() const
{
	return _readFd[0]; 
}

int Client::getSocket() const
{
	return _socketFd;
}

Request& Client::getReq() const
{
	return *_request;
}

const std::stringstream& Client::getResponseBuffer() const
{
    return _responseBuffer;
}