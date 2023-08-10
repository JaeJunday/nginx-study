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
    std::string filePath = findLocationPath();
    checkLimitExcept();
    removeFile(filePath.c_str());
    // checkLimitExcept();
}

void Delete::removeFile(std::string file) const
{
    file = "." + file;
    std::cerr << "file: " << file << std::endl;
    std::remove(file.c_str());
}
