#pragma once

#include <iostream>
#include <string>
#include <map>

struct Location 
{
	const std::string 										_path;
    std::map<const std::string&, std::vector<std::string>> 	_locationProperty;
};