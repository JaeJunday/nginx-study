#include "Client.hpp"

void Client::addSocketReadEvent()
{
    struct kevent event;
    
    EV_SET(&event, _socketFd, EVFILT_READ, EV_ADD, 0, 0, this);
    if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
        throw 500;
}

void Client::addSocketWriteEvent()
{
    struct kevent event;

    EV_SET(&event, _socketFd, EVFILT_WRITE, EV_ADD, 0, 0, this);
    if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
        throw 500;
}

void Client::addPipeReadEvent()
{
    struct kevent event;
    
    EV_SET(&event, _readFd[0], EVFILT_READ, EV_ADD, 0, 0, this);
    if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
        throw 500;
}

void Client::addPipeWriteEvent()
{
    struct kevent event;

    EV_SET(&event, _writeFd[1], EVFILT_WRITE, EV_ADD, 0, 0, this);
    if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
        throw 500;
}

void Client::addProcessEvent()
{
    struct kevent event;

    EV_SET(&event, _pid, EVFILT_PROC, EV_ADD, NOTE_EXIT, 0, this);
    if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
        throw 500;
}

void Client::addTimerEvent()
{
    struct kevent event;

    EV_SET(&event, _socketFd, EVFILT_TIMER, EV_ADD | EV_ENABLE, NOTE_SECONDS, timer::TIMEOUT, this);
    if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
        throw 500;
}
