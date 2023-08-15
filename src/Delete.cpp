#include "Client.hpp"

void Client::deleteProcess() //override
{
    _responseBuffer << _version << " " << _stateCode << " " << _reasonPhrase << "\r\n";
	_responseBuffer << "Date: " << getDate() << "\r\n";
	_responseBuffer << "Server: " << _serverName << "\r\n";
	_responseBuffer << "Content-Type: " << _contentType << "\r\n";
    std::string filePath = findLocationPath();
    checkLimitExcept();
    removeFile(filePath.c_str());
    // checkLimitExcept();
}

void Client::removeFile(std::string file) const
{
    file = "." + file;
    std::cerr << "file: " << file << std::endl;
    std::remove(file.c_str());
}
