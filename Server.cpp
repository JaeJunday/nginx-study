#include "Server.hpp"

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

    for (size_t i = 0; i < _location.size(); ++i)
    {
        if (_location[i]._path == location._path)
            throw std::logic_error("Error: path duplicate");
    }
    _location.push_back(location);
}

const Location& Server::getLocation(int index) const {
    return _location[index];
}

int Server::getLocationSize() const {
    return _location.size();
}