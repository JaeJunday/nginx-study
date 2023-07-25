#pragma once

#include <netinet/in.h> // sockaddr_in
#include <cstdint>
#include <vector>
#include <string>
#include <iostream>

namespace util
{
    std::vector<std::string> getToken(std::string& str, const std::string& delimiters);
    unsigned int convertIp(std::string ipStr);
	unsigned int stoui(const std::string& str);
}
