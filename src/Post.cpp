#include "Post.hpp"
#include "Request.hpp"

// NOTE_EXIT -> 프로세스 종료될때 이벤트 발생
// EV_SET(&event, pid, NOTE_EXIT, EV_ADD, 0, 0, nullptr);
// kevent(kq, &event, 1, NULL, 0, NULL);

Post::Post(Request* request) : AResponse()
{
    _request = request;
}

void Post::createResponse()
{
    _buffer << _version << " " << _stateCode << " " << _reasonPhrase << "\r\n";
	_buffer << "Date: " << getDate() << "\r\n";
	_buffer << "Server: " << _serverName << "\r\n";
	_buffer << "Content-Type: " << _contentType << "\r\n";

	// 함수로 뺄 예정
	int writeFd[2]; // parent(w) -> child(r)
	int readFd[2]; // child(w) -> parent(r)
	struct kevent event;
	int kq2, ret;

    pipe(writeFd);
	pipe(readFd);

	kq2 = kqueue();	
	EV_SET(&event, writeFd[1], EVFILT_WRITE, EV_ADD, 0, 0, nullptr);
	kevent(kq2, &event, 1, NULL, 0, NULL);
	EV_SET(&event, readFd[0], EVFILT_READ, EV_ADD, 0, 0, nullptr);
	kevent(kq2, &event, 1, NULL, 0, NULL);
    pid_t pid = fork();
    if (pid == 0)
    {
		childProcess(writeFd, readFd);
    }
    // 부모 프로세스에서 파이프를 닫습니다.
    close(writeFd[0]);
    close(readFd[1]);
	uploadFile(writeFd[1], kq2);
	close(writeFd[1]);
	std::string result = printResult(readFd[0], kq2);
    close(readFd[0]);
	std::cerr << result << std::endl;
	// 자식 프로세스가 종료될 때까지 기다립니다.
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
	setenv("BOUNDARY", _request->getBoundary().c_str(), true);
	extern char** environ;
	const char* scriptPath = "test4.py";  // 실행할 파이썬 스크립트의 경로
	char* const args[] = {const_cast<char*>("python3"), const_cast<char*>(scriptPath), nullptr};
	// execve 함수로 파이썬 스크립트 실행
	if (execve("/usr/bin/python3", args, environ) == -1) {
		perror("execve");  // 오류 처리
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
			// 여기서 데이터를 적절하게 처리하거나 필요한 크기로 분할하여 처리
			// 예를 들면 데이터를 파일에 쓰는 것도 가능
			write(fd, data.c_str() + size, writeSize);
			size += tevent.data;
		}
	}
}

const std::string& Post::printResult(int fd, int kq)
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

