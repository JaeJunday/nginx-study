#include "Server.hpp"

Server::Server()
     //_serverName(0),
      //_errorPage(0),
      //_listen(0),
      //_index(0),
      //_clientMaxBodySize(0)
      //_location(0)
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

void Server::setValue(int index, std::string& value)
{
    switch (index)
    {
        case server::NAME:
            _serverName.push_back(value); break;
        case server::ROOT:
            _root = value; break;
        case server::LISTEN:
            _listen = value; break;
        case server::ERROR: 
            _errorPage.push_back(value); break;
        case server::INDEX:
            _index =  value; break;
        case server::MAXBODYSIZE:
            _clientMaxBodySize = value; break;
    }
}

std::string Server::getValue(int index) const
{
    switch (index)
    {
        case server::NAME:
            return (_serverName[index]);
        case server::ROOT:
            return (_root);
        case server::LISTEN:
            return (_listen);
        case server::ERROR:
            return _serverName[index];
            return (_errorPage[index]);
        case server::INDEX:
            return (_index);
        case server::MAXBODYSIZE:
            return (_clientMaxBodySize);
    }
    return NULL;
}

void Server::setLocation(const Location& location) {
    _location.push_back(location);
}

const Location& Server::getLocation(int index) const {
    return _location[index];
}

int Server::getLocationSize() const {
    return _location.size();
}