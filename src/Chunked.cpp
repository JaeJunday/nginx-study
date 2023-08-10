#include "Chunked.hpp"

// NOTE_EXIT -> 프로세스 종료될때 이벤트 발생
// EV_SET(&event, pid, NOTE_EXIT, EV_ADD, 0, 0, nullptr);
// kevent(kq, &event, 1, NULL, 0, NULL);

Chunked::Chunked(Request* request, int kq) : AResponse(kq), _pid(-2)
{
	_request = request;
}

void Chunked::createResponse()
{
    std::vector<std::string> url = util::getToken(_request->getRequestUrl(), "/");
    if (url.size() >= 1)
        _chunkedFilename = url[url.size() - 1];
	pipe(_writeFd);
	pipe(_readFd);

	util::setEvent(_writeFd[1], _kq, EVFILT_WRITE);
	util::setEvent(_readFd[0], _kq, EVFILT_READ);

	_pid = fork();
	if (_pid == 0)
		childProcess();
	close(_writeFd[0]);
	close(_readFd[1]);
}

void Chunked::endResponse()
{
	close(_writeFd[1]);
	waitpid(_pid, NULL, 0);
	std::string result = printResult(_readFd[0], _kq);
	close(_readFd[0]);	
	_buffer << result;
	findLocationPath();
	checkLimitExcept();
}

void Chunked::childProcess()
{
	dup2(_writeFd[0], STDIN_FILENO);
	close(_writeFd[0]);
	close(_writeFd[1]);
	dup2(_readFd[1], STDOUT_FILENO);
	close(_readFd[0]);
	close(_readFd[1]);
	const char* scriptPath = "./src/cgi/chunked_upload_cgi.py";  // 실행할 파이썬 스크립트의 경로
	char* const args[] = {const_cast<char*>("python3"), const_cast<char*>(scriptPath), nullptr};
	setenv("FILENAME", _chunkedFilename.c_str(), true);
	setenv("CONTENT_TYPE", _request->getContentType().c_str(), true);
	extern char** environ;
	if (execve("/usr/bin/python3", args, environ) == -1) {
		perror("execve");  // 오류 처리
	}
}

void Chunked::uploadFile(const std::string& body)
{
	struct kevent	tevent;
	int				ret;
	int				size = 0;
	std::string		data = body;
	size_t			writeSize;

	while (true)
	{
		ret = kevent(_kq, nullptr, 0, &tevent, 1, nullptr);
		if (ret == -1) 
			std::cerr << "kevent error: " << std::strerror(errno) << std::endl;
		if (tevent.ident != _writeFd[1])
			continue;
		if (size >= data.length())
			return;
		if (tevent.filter == EVFILT_WRITE)
		{
			writeSize = std::min((size_t)tevent.data, data.size() - size);
			write(_writeFd[1], data.c_str() + size, writeSize);
			size += writeSize;
		}
	}
}

const std::string Chunked::printResult(int fd, int _kq)
{
	struct	kevent tevent;
	size_t	size = 0;
	size_t	readSize;
	int		ret;
	char tempBuffer[PIPESIZE];
	std::string readBuffer;
	memset(tempBuffer, 0, PIPESIZE);

	while (true)
	{
		ret = kevent(_kq, nullptr, 0, &tevent, 1, nullptr);
		//testcode
		if (ret == -1) 
			std::cerr << "kevent error: " << std::strerror(errno) << std::endl;
		if (tevent.ident != fd)
			continue;
		if (tevent.filter == EVFILT_READ)
		{
			while (true)
			{
				readSize = read(fd, tempBuffer, PIPESIZE);
				if (readSize < 0)
					break;
				readBuffer.append(tempBuffer, readSize);
				if (readSize == 0)
					return readBuffer;
			}
		}
	}
	return readBuffer;
}

pid_t Chunked::getPid() const
{
	return _pid;
}