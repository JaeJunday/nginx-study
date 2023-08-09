#include "Chunked.hpp"

// NOTE_EXIT -> 프로세스 종료될때 이벤트 발생
// EV_SET(&event, pid, NOTE_EXIT, EV_ADD, 0, 0, nullptr);
// kevent(kq, &event, 1, NULL, 0, NULL);

Chunked::Chunked(Request* request, int kq) : AResponse(kq)
{
    _request = request;
}

void Chunked::createResponse()
{
	int writeFd[2]; // parent(w) -> child(r)
	int readFd[2]; // child(w) -> parent(r)
	struct kevent event;
	int ret;

    pipe(writeFd);
	pipe(readFd);
	util::setEvent(writeFd[1], _kq, EVFILT_WRITE);
	util::setEvent(readFd[0], _kq, EVFILT_READ);

    // 몇번째 호출인지 확인해서 
    pid_t pid = fork();
    if (pid == 0)
		childProcess(writeFd, readFd);

    close(writeFd[0]);
    close(readFd[1]);
	uploadFile(writeFd[1], _kq);
	close(writeFd[1]);
	std::string result = printResult(readFd[0], _kq);
    close(readFd[0]);
    waitpid(pid, NULL, 0);
	_buffer << result;
}

void Chunked::childProcess(int *writeFd, int *readFd)
{
	dup2(writeFd[0], STDIN_FILENO);
	close(writeFd[0]);
	close(writeFd[1]);
	dup2(readFd[1], STDOUT_FILENO); close(readFd[0]);
	close(readFd[1]);
	const char* scriptPath = "./src/cgi/chunked_upload_cgi.py";  // 실행할 파이썬 스크립트의 경로
	char* const args[] = {const_cast<char*>("python3"), const_cast<char*>(scriptPath), nullptr};
	setenv("FILENAME", _request->getChunkedFilename().c_str(), true);
	setenv("CONTENT_TYPE", _request->getContentType().c_str(), true);
	extern char** environ;
	if (execve("/usr/bin/python3", args, environ) == -1) {
		perror("execve");  // 오류 처리
	}
}

void Chunked::uploadFile(int fd, int kq)
{
	struct kevent	tevent;
	int				ret;
	int				size = 0;
	std::string		data = _request->getBuffer();
	size_t			writeSize;

	std::cerr << RED << "testcode " << "data"  << data << RESET << std::endl;

	while (true)
	{
		ret = kevent(kq, nullptr, 0, &tevent, 1, nullptr);
		if (ret == -1) 
			std::cerr << "kevent error: " << std::strerror(errno) << std::endl;
		if (tevent.ident != fd)
			continue;
		if (size >= data.length())
			break;
		if (tevent.filter == EVFILT_WRITE)
		{
			writeSize = std::min((size_t)tevent.data, data.size() - size);
			write(fd, data.c_str() + size, writeSize);
			size += writeSize;
		}
	}
}

const std::string Chunked::printResult(int fd, int kq)
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
		ret = kevent(kq, nullptr, 0, &tevent, 1, nullptr);
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

