#include "Util.hpp"
// #include "Request.hpp"

std::vector<std::string> util::getToken(const std::string& str, const std::string& delimiters)
{
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = 0;

    while (end != std::string::npos) {
        end = str.find(delimiters, start);
        if (end != start) 
        {
            std::string tmp = str.substr(start, (end == std::string::npos) ? std::string::npos : end - start);
            if (tmp.empty() == false)
                result.push_back(tmp);
        }
        if (end == std::string::npos) 
            break;
        start = end + 1;
    }
    return result;
}

uint32_t util::stoui(const std::string& str)
{
    return static_cast<uint32_t>(std::strtod(str.c_str(), NULL));
}

/*
uint32_t util::convertIp(std::string& ipStr)
{
    std::vector<std::string> tmp = util::getToken(ipStr, ".");
    if (tmp.size() != OCTET_COUNT)
        throw std::runtime_error("Error: Invalid Ip Address");
    uint32_t ip = 0;
    for (int i = 0, shift = 24; i < tmp.size(); ++i, shift -= 8) { 
        uint32_t octet = util::stoui(tmp[i]);
        if (octet > OCTET_MAX)
            throw std::runtime_error("Error: Invalid Octet Range 0 ~ 255");
        ip |= octet << shift;
    }
    return ip;
}
*/