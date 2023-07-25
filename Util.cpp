#include "Util.hpp"

std::vector<std::string> util::getToken(std::string& str, const std::string& delimiters)
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

unsigned int util::stoui(const std::string& str)
{
    return static_cast<unsigned int>(std::strtod(str.c_str(), NULL));
}

unsigned int util::convertIp(std::string ipStr)
{
    std::vector<std::string> tmp = util::getToken(ipStr, ".");
    int octet;
    unsigned int ip = 0;
    for (int i = 0, shift = 24; i < tmp.size(); ++i, shift -= 8) { 
        octet = util::stoui(tmp[i]);
        if (std::isdigit(octet) == false)
            throw std::runtime_error("Error: Octet is not digit");
        if (octet < 0 && octet > 255)
            throw std::runtime_error("Error: Invalid Octet Range 0 ~ 255");
        ip |= octet << shift;
    }
    return ip;
}
