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
    checkLimitExcept();
}

void Delete::checkLimitExcept() const
{
    std::string filePath = findLocationPath();
    std::cerr << "path: " << filePath << std::endl;
    std::string limit = _request->getLocation()->_limitExcept;
    std::cerr << "limit: " << limit << std::endl;

    // if (limit.empty())
    //     throw 405; // Method Not Allowed
    std::vector<std::string> limits = util::getToken(limit, " ");
    for (int i = 0; i < limits.size(); ++i) 
    {
        if (limits[i] == "DELETE")
        {
            removeFile(filePath.c_str());
            return;
        }
    }
    // throw 405; 
}

void Delete::removeFile(std::string file) const
{
    file = "." + file;
    std::cerr << "file: " << file << std::endl;
    std::remove(file.c_str());
}

