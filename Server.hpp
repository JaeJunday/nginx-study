#pragma once

#include <iostream>
#include <locale>
#include <string>
#include <map>
#include <vector>

class Operation;

struct Location
{
	const std::string	_path;
	const std::string	_root;
	const std::string	_index;
	const std::string	_autoindex;
	const std::string	_upload;
	const std::string	_py;
	const std::string	_php;
	const std::string	_clientMaxBodySize; // 본문 사이즈 제한
	const std::string	_limitExcept; // 메서드 제한
	const std::string	_tryFiles;
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
		void setServerName(const std::string& serverName);
		const std::string& getServerName() const;

		void setErrorPage(const std::string& errorPage);
		const std::string& getErrorPage() const;

		void setListen(const std::string& listen);
		const std::string& getListen() const;

		void setIndex(const std::string& index);
		const std::string& getIndex() const;

		void setClientMaxBodySize(const std::string& clientMaxBodySize);
		const std::string& getClientMaxBodySize() const;

		void setLocation(const std::string& location);
		const std::string& getLocation() const;
};
