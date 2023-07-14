#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
struct Location 
{
	const std::string _path;
};
class Server {
private:
	std::string& 		_serverName;
	std::string&		_listen;
	std::vector<Location>									_location;
public:
	void	setProperty(const std::string& key, std::vector<std::string>& value);
	void	setLocation(void);
};
