#include "Delete.hpp"
#include "Request.hpp"

Delete::Delete(Request* request, int kq) : AResponse(kq)
{
    _request = request;
}

void Delete::createResponse() //override
{
    _buffer << _version << " " << _stateCode << " " << _reasonPhrase << "\r\n";
	_buffer << "Date: " << getDate() << "\r\n";
	_buffer << "Server: " << _serverName << "\r\n";
	_buffer << "Content-Type: " << _contentType << "\r\n";

    removeFile();
}

void Delete::removeFile() const
{
}