#pragma once

#include "Location.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>

class Server {
private:
    std::map<const std::string&, std::vector<std::string>>	_property;
	std::vector<Location>									_location;
public:
	void	setProperty(const std::string& key, std::vector<std::string>& value);
	void	setLocation(void);
};