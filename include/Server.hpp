#pragma once

#include "enum.hpp"
#include "Util.hpp"
#include <string>
#include <vector>

class Operation;

struct Location
{
	std::string					_path;
	std::string					_root;
	std::string					_index;
	std::string					_autoindex;
	std::string					_upload;
	std::string					_py;
	std::string 				_bla;
	std::string					_php;
	std::string					_clientMaxBodySize;
	std::vector<std::string>	_limitExcept;
	std::string					_tryFiles;
};

class Server {
	private:
		std::vector<std::string>	_serverName;
		std::vector<std::string>  	_errorPage;
		uint32_t					_listen;
		std::string					_root;
		std::string					_index;
		std::string					_clientMaxBodySize;
		std::vector<Location>		_location;
		uint32_t 					_socket;
	public:
		Server();
		Server(const Server& other);
		Server& operator=(const Server& other);
		~Server();
		// setter
		void setValue(int index, std::string& value);
		void setLocation(const Location& location);
		// getter
		const Location& getLocation(int index) const;
		int getLocationSize() const;
		void setSocket(int fd);
		int getSocket() const;
		const std::string& getRoot() const;
		const std::string& getIndex() const;
		const std::vector<Location>* getLocations() const;
		std::vector<std::string>& getServerName();
		uint32_t getListen();
};
