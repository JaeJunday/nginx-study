#pragma once


#include <netinet/in.h> // sockaddr_in
#include <cstdint>
#include <cctype>
#include <vector>
#include <string>
#include <iostream>
#include <sys/event.h>  // kqueue

#define OCTET_MAX 255
#define OCTET_COUNT 4

class Request;

namespace util
{
    std::vector<std::string> getToken(const std::string& str, const std::string& delimiters);
    uint32_t convertIp(std::string& ipStr);
	uint32_t stoui(const std::string& str);
    // void addEvent(Client* client, int kq, int filter);
    // void addEvent(int fd, int kq, int filter);
}
