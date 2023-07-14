#include "Server.hpp"

void Server::setServerName(const std::vector<std::string>& serverName) {
    _serverName = serverName;
}

const std::vector<std::string>& Server::getServerName() const {
    return _serverName;
}

void Server::setErrorPage(const std::vector<std::string>& errorPage) {
    _errorPage = errorPage;
}

const std::vector<std::string>& Server::getErrorPage() const {
    return _errorPage;
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

void Server::setLocation(const std::vector<Location>& location) {
    _location = location;
}

const std::vector<Location>& Server::getLocation() const {
    return _location;
}
