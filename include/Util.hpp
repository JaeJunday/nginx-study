#pragma once

#include "enum.hpp"
#include <cstdint>
#include <cctype>
#include <vector>
#include <string>
#include <iostream>
#include <sys/event.h>

#define OCTET_MAX 255
#define OCTET_COUNT 4

class Request;

namespace util
{
	std::vector<std::string> getToken(const std::string& str, const std::string& delimiters);

	uint32_t	stoui(const std::string& str);
	std::string	getDate();
	std::string	findContentType(const std::string& filePath);
	void		checkMultipleSpaces(const std::string& str);
}
