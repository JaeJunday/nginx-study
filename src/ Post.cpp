#include "Post.hpp"
#include "Request.hpp"

std::putenv(const_cast<char*>(env_var));

void Post::createResponse()
{
    int fd[2];
    pipe(fd);

    pid_t id = fork();
    if (id == 0)
    {
        dup2(fd[0], STDIN_FILENO);
        // dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        // close(fd[1]);

	    std::string data = _request->getBuffer();
    	write(fd[1], data.c_str(), data.length());
        close(fd[1]);


        const char* scriptPath = "test3.py";  // 실행할 파이썬 스크립트의 경로
        char* const args[] = {const_cast<char*>("python"), const_cast<char*>(scriptPath), nullptr};

        // execve 함수로 파이썬 스크립트 실행
        if (execve("/usr/bin/python3", args, nullptr) == -1) {
            perror("execve");  // 오류 처리
        }
        exit(1);
    }

    // 부모 프로세스에서 파이프를 닫습니다.
    close(fd[0]);
    close(fd[1]);
    // 자식 프로세스가 종료될 때까지 기다립니다.
    waitpid(id, NULL, 0);
}