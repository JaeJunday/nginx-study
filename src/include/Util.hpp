#pragma once

#include <netinet/in.h> // sockaddr_in
#include <cstdint>
#include <cctype>
#include <vector>
#include <string>
#include <iostream>

#define OCTET_MAX 255
#define OCTET_COUNT 4
namespace util
{
    std::vector<std::string> getToken(std::string& str, const std::string& delimiters);
    uint32_t convertIp(std::string& ipStr);
	uint32_t stoui(const std::string& str);
}
