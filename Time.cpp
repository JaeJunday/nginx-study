#include "Time.hpp"

void Time::stamp()
{
    std::time_t now = std::time(NULL);
    char timeStamp[TIME_SIZE];
    std::strftime(timeStamp, sizeof(timeStamp), "Date: %a, %d %b %Y %H:%M:%S GMT", std::localtime(&now));
    std::cout << timeStamp << std::endl;
}