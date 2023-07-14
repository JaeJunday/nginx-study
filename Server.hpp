#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
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
	std::vector<std::string>	_errorPage;
	const std::string			_listen;
	const std::string			_index;
	const std::string			_clientMaxBodySize;
	// location block	
	std::vector<Location>		_location;
public:
	void	setProperty(const std::string& key, std::vector<std::string>& value);
	void	setLocation(void);
};
