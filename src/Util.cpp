#include "Util.hpp"
#include "Request.hpp"

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

//소켓 전용 event setter
void util::setEvent(Client* client, int kq, int filter)
{
    struct kevent event;

    // DELETE
    if (req->getEventState() == event::READ && filter == event::WRITE)
    {
        EV_SET(&event, req->getSocket(), EVFILT_READ, EV_DELETE, 0, 0, client);
        kevent(kq, &event, 1, NULL, 0, NULL);
    }
    else if (req->getEventState() == event::WRITE && filter == event::READ)
    {
        EV_SET(&event, req->getSocket(), EVFILT_WRITE, EV_DELETE, 0, 0, client);
        kevent(kq, &event, 1, NULL, 0, NULL);
    }
    // ADD
    if (filter == event::READ)
    {
        EV_SET(&event, req->getSocket(), EVFILT_READ, EV_ADD, 0, 0, client);
        if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
            std::cerr << "invalid Read event set 1" << std::endl;
        req->setEventState(event::READ);
    }
    else if (filter == event::WRITE)
    {
        EV_SET(&event, req->getSocket(), EVFILT_WRITE, EV_ADD, 0, 0, client);
        if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
            std::cerr << "invalid Write event set 1" << std::endl;
        req->setEventState(event::READ);
    }
}

//pipe 전용 event setter
// void util::setEvent(int fd, int kq, int filter)
// {
//     struct kevent event;

//     // ADD
//     if (filter == event::READ)
//     {
//         EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
//         if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
//             std::cerr << "invalid Read event set 2" << std::endl;
//     }
//     else if (filter == event::WRITE)
//     {
//         EV_SET(&event, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
//         if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
//             std::cerr << "invalid Write event set 2" << std::endl;
//     }
// }
