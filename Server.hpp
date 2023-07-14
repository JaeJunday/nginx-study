#pragma once

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
		std::string					_index;
		std::string					_clientMaxBodySize;
		std::vector<Location>		_location;
	public:
		Server();
		Server(const Server& other);
		Server& operator=(const Server& other);
		~Server();

		void setServerName(const std::string& serverName);
		const std::string& getServerName(int index) const;

		void setErrorPage(const std::string& errorPage);
		const std::string& getErrorPage(int index) const;

		void setListen(const std::string& listen);
		const std::string& getListen() const;

		void setIndex(const std::string& index);
		const std::string& getIndex() const;

		void setClientMaxBodySize(const std::string& clientMaxBodySize);
		const std::string& getClientMaxBodySize() const;

		void setLocation(const Location& location);
		const Location& getLocation(int index) const;
};
