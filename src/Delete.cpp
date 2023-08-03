#include "Delete.hpp"
#include "Request.hpp"

Delete::Delete(Request* request) : AResponse()
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

void Delete::checkLimitExcept() const
{
    // 1. location 의 limit_except 를 확인한다.
    // 2. limit_except 가 없다면, 405 Method Not Allowed 를 반환한다.

    const std::string limit = _request->getLocation()->_limitExcept;
    if (limit.empty())
        throw 405;
    std::vector<std::string> limits = util::getToken(limit, " ");
    for (int i = 0; i < limits.size(); ++i) 
    {
        if (limits[i] == "DELETE")
            removeFile();
    }
}

void Delete::removeFile() const
{
    const std::string path = _request->getRequestUrl();

    std::remove(path.c_str());
}

