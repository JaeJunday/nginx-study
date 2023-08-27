#include "Client.hpp"

void Client::deleteProcess()
{
    _responseBuffer << _version << " " << _stateCode << " " << _reasonPhrase << "\r\n";
	_responseBuffer << "Date: " << util::getDate() << "\r\n";
	_responseBuffer << "Server: " << _serverName << "\r\n";
	_responseBuffer << "Content-Type: " << _contentType << "\r\n";
    _responseStr = _responseBuffer.str();
    // std::string filePath = findLocationPath();
    // checkLimitExcept();
    removeFile(_request->getConvertRequestPath());
}

void Client::removeFile(std::string file) const
{
    file = "." + file;
    std::cerr << "file: " << file << std::endl;
    std::remove(file.c_str());
}
