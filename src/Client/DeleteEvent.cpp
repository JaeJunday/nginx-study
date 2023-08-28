#include "Client.hpp"

void Client::deleteSocketReadEvent()
{
    struct kevent event;

	EV_SET(&event, _socketFd, EVFILT_READ, EV_DELETE, 0, 0, this);
	if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
		throw 500;
}

void Client::deleteSocketWriteEvent()
{
    struct kevent event;

	EV_SET(&event, _socketFd, EVFILT_WRITE, EV_DELETE, 0, 0, this);
	if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
		throw 500;
}

void Client::deletePidEvent()
{
    struct kevent event;

	EV_SET(&event, _pid, EVFILT_PROC, EV_DELETE, NOTE_EXIT, 0, this);
	if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
		throw 500;
}

void Client::deleteTimerEvent()
{
	struct kevent event;

	EV_SET(&event, _socketFd, EVFILT_TIMER, EV_DELETE, NOTE_EXIT, 0, this);
	if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
		throw 500;
}

void Client::resetTimerEvent()
{
    struct kevent event;

	EV_SET(&event, _socketFd, EVFILT_TIMER, EV_ADD | EV_ENABLE, NOTE_SECONDS, timer::TIMEOUT, this);
	if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
		throw 500;
}