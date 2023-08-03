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
	struct kevent tevent;
	int kq2, ret;
	int size = 0;
	std::string data = _request->getBuffer();

	// std::cerr << "HERE!!!POST!!!!!" << std::endl;
    pipe(writeFd);
	kq2 = kqueue();
	// EV_SET(&event, fd[0], EVFILT_READ, EV_ADD, 0, 0, nullptr);
	// kevent(kq2, &event, 1, NULL, 0, NULL);
	EV_SET(&event, writeFd[1], EVFILT_WRITE, EV_ADD, 0, 0, nullptr);
	kevent(kq2, &event, 1, NULL, 0, NULL);
    pid_t pid = fork();
    if (pid == 0)
    {
        dup2(writeFd[0], STDIN_FILENO);
        close(writeFd[0]);
		close(writeFd[1]);

		// std::string newEnv = "BOUNDARY=" + _request->getBoundary();
		setenv("BOUNDARY", _request->getBoundary().c_str(), true);
		extern char** environ;
        const char* scriptPath = "test4.py";  // 실행할 파이썬 스크립트의 경로
        char* const args[] = {const_cast<char*>("python3"), const_cast<char*>(scriptPath), nullptr};

        // execve 함수로 파이썬 스크립트 실행
        if (execve("/usr/bin/python3", args, environ) == -1) {
            perror("execve");  // 오류 처리
        }
    }
    // 부모 프로세스에서 파이프를 닫습니다.
    close(writeFd[0]);
	while (true)
	{
		ret = kevent(kq2, nullptr, 0, &tevent, 1, nullptr);
		if (ret == -1) 
			std::cerr << "kevent error: " << std::strerror(errno) << std::endl;
		if (size >= data.length())
			break;
		if (tevent.filter == EVFILT_WRITE)
		{
			// 여기서 데이터를 적절하게 처리하거나 필요한 크기로 분할하여 처리
			// 예를 들면 데이터를 파일에 쓰는 것도 가능
			write(writeFd[1], data.c_str() + size, tevent.data);
			size += tevent.data;
		}
	}
    close(writeFd[1]);
    // 자식 프로세스가 종료될 때까지 기다립니다.
    waitpid(pid, NULL, 0);
}
