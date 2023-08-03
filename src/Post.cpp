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
	int fd[2];
    pipe(fd);

	std::cerr << "HERE!!!POST!!!!!" << std::endl;

    pid_t pid = fork();
    if (pid == 0)
    {
        dup2(fd[0], STDIN_FILENO);
        // dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        // close(fd[1]);

	    std::string data = _request->getBuffer();
    	write(fd[1], data.c_str(), data.length());
        close(fd[1]);
		// std::string newEnv = "BOUNDARY=" + _request->getBoundary();
		setenv("BOUNDARY", _request->getBoundary().c_str(), true);

		extern char** environ;
  		// char* env_var = *environ;
   		//  while (env_var != nullptr) {
		// 	std::cout << env_var << std::endl;
		// 	env_var = *(environ++);
    	// }


        const char* scriptPath = "test4.py";  // 실행할 파이썬 스크립트의 경로
        char* const args[] = {const_cast<char*>("python3"), const_cast<char*>(scriptPath), nullptr};

        // execve 함수로 파이썬 스크립트 실행
        if (execve("/usr/bin/python3", args, environ) == -1) {
            perror("execve");  // 오류 처리
        }
        exit(1);
    }
    // 부모 프로세스에서 파이프를 닫습니다.
    close(fd[0]);
    close(fd[1]);
    // 자식 프로세스가 종료될 때까지 기다립니다.
    waitpid(pid, NULL, 0);
	
}
