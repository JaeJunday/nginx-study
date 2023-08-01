#pragma once

#include "enum.hpp"
#include <iostream>
#include <locale>
#include <string>
#include <map>
#include <vector>

class Operation;

struct Location
{
	std::string	_path;
	std::string	_root;
	std::string	_index;
	std::string	_autoindex;
	std::string	_upload;
	std::string	_py;
	std::string	_php;
	std::string	_clientMaxBodySize; // 본문 사이즈 제한
	std::string	_limitExcept; // 메서드 제한
	std::string	_tryFiles;
};
class Server {
	private:
		std::vector<std::string>	_serverName;
		std::vector<std::string>  	_errorPage;
		std::string					_listen;
		std::string					_root;
		std::string					_index;
		std::string					_clientMaxBodySize;
		std::vector<Location>		_location;
    	int 						_socket;
	public:
		// Server();
		// Server(const Server& other);
		Server& operator=(const Server& other);
		void setValue(int index, std::string& value);
		std::string getValue(int index) const;
		void setLocation(const Location& location);
		const Location& getLocation(int index) const;
		int getLocationSize() const;
		void setSocket(int fd);
		int getSocket() const;
		const std::vector<Location>& getLocations() const;
};
