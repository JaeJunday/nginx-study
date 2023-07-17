#include "Server.hpp"

Server::Server()
    : _serverName(0),
      _errorPage(0),
      _listen(0),
      _index(0),
      _clientMaxBodySize(0),
      _location(0)
{
}

Server::Server(const Server& other)
    : _serverName(other._serverName),
      _errorPage(other._errorPage),
      _listen(other._listen),
      _index(other._index),
      _clientMaxBodySize(other._clientMaxBodySize),
      _location(other._location)
{
}

Server& Server::operator=(const Server& other)
{
    if (this != &other)
    {
        _serverName = other._serverName;
        _errorPage = other._errorPage;
        _listen = other._listen;
        _index = other._index;
        _clientMaxBodySize = other._clientMaxBodySize;
        _location = other._location;
    }
    return *this;
}

Server::~Server()
{
}

void Server::setServerName(const std::string& serverName) {
	_serverName.push_back(serverName);
}

const std::string& Server::getServerName(int index) const {
    return _serverName[index];
}

void Server::setErrorPage(const std::string& errorPage) {
    _errorPage.push_back(errorPage);
}

const std::string& Server::getErrorPage(int index) const {
    return _errorPage[index];
}

void Server::setListen(const std::string& listen) {
    _listen = listen;
}

const std::string& Server::getListen() const {
    return _listen;
}

void Server::setIndex(const std::string& index) {
    _index = index;
}

const std::string& Server::getIndex() const {
    return _index;
}

void Server::setClientMaxBodySize(const std::string& clientMaxBodySize) {
    _clientMaxBodySize = clientMaxBodySize;
}

const std::string& Server::getClientMaxBodySize() const {
    return _clientMaxBodySize;
}

void Server::setLocation(const Location& location) {
    _location.push_back(location);
}

const Location& Server::getLocation(int index) const {
    return _location[index];
}
