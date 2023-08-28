#include "Server.hpp"

Server::Server()
	: _socket(-1)
{}

Server::Server(const Server& other)
	: _serverName(other._serverName),
	  _errorPage(other._errorPage),
	  _listen(other._listen),
	  _root(other._root),
	  _index(other._index),
	  _clientMaxBodySize(other._clientMaxBodySize),
	  _location(other._location),
	  _socket(other._socket)
{}

Server& Server::operator=(const Server& other)
{
	if (this != &other)
	{
		_serverName = other._serverName;
		_errorPage = other._errorPage;
		_listen = other._listen;
		_root = other._root;
		_index = other._index;
		_clientMaxBodySize = other._clientMaxBodySize;
		_location = other._location;
		_socket = other._socket;
	}
	return *this;
}

Server::~Server(){}

void Server::setValue(int index, std::string& value)
{
	switch (index)
	{
		case server::NAME:
			_serverName.push_back(value); break;
		case server::ROOT:
			_root = value; break;
		case server::LISTEN:
        	_listen = util::stoui(value); 
			if (_listen == 0)
				throw std::logic_error("Error: listen is invalid.");
			break;
		case server::ERROR:
			_errorPage.push_back(value); break;
		case server::INDEX:
			_index =  value; break;
		case server::MAXBODYSIZE:
			_clientMaxBodySize = value; break;
	}
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

const std::vector<Location>* Server::getLocations() const 
{
	return &_location;
}

int Server::getLocationSize() const {
	return _location.size();
}

void Server::setSocket(int fd)
{
	_socket = fd;
}

int Server::getSocket() const
{
	return _socket;
}
		
const std::string& Server::getRoot() const
{
	return _root;
}

const std::string& Server::getIndex() const
{
	return _index;
}

std::vector<std::string>& Server::getServerName()
{
    return _serverName;
}

uint32_t Server::getListen()
{
	return _listen;
}