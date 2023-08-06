#include "Post.hpp"
#include "Request.hpp"

// NOTE_EXIT -> 프로세스 종료될때 이벤트 발생
// EV_SET(&event, pid, NOTE_EXIT, EV_ADD, 0, 0, nullptr);
// kevent(kq, &event, 1, NULL, 0, NULL);

Post::Post(Request* request, int kq) : AResponse(kq)
{
    _request = request;
}

void Post::createResponse()
{
	checkLimitExcept();
    _buffer << _version << " " << _stateCode << " " << _reasonPhrase << "\r\n";
	_buffer << "Date: " << getDate() << "\r\n";
	_buffer << "Server: " << _serverName << "\r\n";
	// 이거 나중에 넣는걸로 바꿔줘야함
	_buffer << "Content-Type: " << _contentType << "\r\n";

	int writeFd[2]; // parent(w) -> child(r)
	int readFd[2]; // child(w) -> parent(r)
	struct kevent event;
	int ret;

    pipe(writeFd);
	pipe(readFd);
	util::setEvent(writeFd[1], _kq, EVFILT_WRITE);
	util::setEvent(readFd[0], _kq, EVFILT_READ);
    pid_t pid = fork();
    if (pid == 0)
		childProcess(writeFd, readFd);
    close(writeFd[0]);
    close(readFd[1]);
	uploadFile(writeFd[1], _kq);
	close(writeFd[1]);
	std::string result = printResult(readFd[0], _kq);
    close(readFd[0]);
	std::cerr << result << std::endl;
    waitpid(pid, NULL, 0);
}

void Post::childProcess(int *writeFd, int *readFd)
{
	dup2(writeFd[0], STDIN_FILENO);
	close(writeFd[0]);
	close(writeFd[1]);
	dup2(readFd[1], STDOUT_FILENO);
	close(readFd[0]);
	close(readFd[1]);
	if (_request->getTransferEncoding() == "chunked")
	{
		const char* scriptPath = "./src/cgi/chunked_upload_cgi.py";  // 실행할 파이썬 스크립트의 경로
		char* const args[] = {const_cast<char*>("python3"), const_cast<char*>(scriptPath), nullptr};
		setenv("FILENAME", _request->getChunkedFilename().c_str(), true);
		setenv("CONTENT_TYPE", _request->getContentType().c_str(), true);
		extern char** environ;
		if (execve("/usr/bin/python3", args, environ) == -1) {
			perror("execve");  // 오류 처리
		}
	}
	else
	{
		const char* scriptPath = "./src/cgi/upload_cgi.py";  // 실행할 파이썬 스크립트의 경로
		char* const args[] = {const_cast<char*>("python3"), const_cast<char*>(scriptPath), nullptr};
		setenv("BOUNDARY", _request->getBoundary().c_str(), true);
		extern char** environ;
		// execve 함수로 파이썬 스크립트 실행
		if (execve("/usr/bin/python3", args, environ) == -1) {
			perror("execve");  // 오류 처리
		}
	}

}

void Post::uploadFile(int fd, int kq)
{
	struct kevent	tevent;
	int				ret;
	int				size = 0;
	std::string		data = _request->getBuffer();
	size_t			writeSize;

	while (true)
	{
		ret = kevent(kq, nullptr, 0, &tevent, 1, nullptr);
		if (ret == -1) 
			std::cerr << "kevent error: " << std::strerror(errno) << std::endl;
		if (size >= data.length())
			break;
		if (tevent.filter == EVFILT_WRITE)
		{
			writeSize = std::min((size_t)tevent.data, data.size() - size);
			write(fd, data.c_str() + size, writeSize);
			size += tevent.data;
		}
	}
}

const std::string Post::printResult(int fd, int kq)
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
		if (ret == -1) 
			std::cerr << "kevent error: " << std::strerror(errno) << std::endl;
		if (tevent.filter == EVFILT_READ)
		{
			while (1)
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

