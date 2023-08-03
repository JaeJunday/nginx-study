#include "Post.hpp"
#include "Request.hpp"

Post::Post(Request* request) : AResponse()
{
    _request = request;
}

void Post::createResponse()
{
    _buffer << _version << " " << _stateCode << " " << _reasonPhrase << "\r\n";
	_buffer << "Date: " << getDate() << "\r\n";
	_buffer << "Server: " << _serverName << "\r\n";
	_buffer << "Content-Type: " << _contentType << "\r\n";
}

void Post::createResponseMain() // override
{
}
