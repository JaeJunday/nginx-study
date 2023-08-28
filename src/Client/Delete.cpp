#include "Client.hpp"

void Client::handleDelete()
{
	_responseBuffer << _version << " " << _stateCode << " " << _reasonPhrase << "\r\n";
	_responseBuffer << "Date: " << util::getDate() << "\r\n";
	_responseBuffer << "Server: " << _serverName << "\r\n";
	_responseBuffer << "Content-Type: " << _contentType << "\r\n\r\n";
	_responseStr = _responseBuffer.str();
	removeFile(_request->getConvertRequestPath());
	deleteSocketReadEvent();
	addSocketWriteEvent();
}

void Client::removeFile(std::string file) const
{
	file = "." + file;
	int isRemove = std::remove(file.c_str());	
	if (isRemove == -1)
		throw 404;
}