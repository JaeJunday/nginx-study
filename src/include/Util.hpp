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
    /*  kyeonkim
        @func_name convertIp
        @deprecated 192.0.0.1 라는 아이피를 넣어서 bind 해보려고 테스트 해봤지만 안됨. 
        되면 되는 테스트 케이스가 필요한 듯(192.0.0.1:80, 168.0.0.1:80 이 두 개의 ip:port가 서로 다르게 bind 돼야 처리가능).
        현재 0.0.0.0 IP만 사용 중.
        (우선 주석)
    */
    // uint32_t convertIp(std::string& ipStr);
	uint32_t stoui(const std::string& str);
}
