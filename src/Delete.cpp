#include "Delete.hpp"
#include "Request.hpp"

Delete::Delete(Request* request) : AResponse()
{
    _request = request;
}

void Delete::createResponseHeader(std::vector<Server> servers)
{
    _buffer << _version << " " << _stateCode << " " << _reasonPhrase << "\r\n";
	_buffer << "Date: " << getDate() << "\r\n";
	_buffer << "Server: " << _serverName << "\r\n";
	_buffer << "Content-Type: " << _contentType << "\r\n";
}

void Delete::createResponseMain() // override
{
}