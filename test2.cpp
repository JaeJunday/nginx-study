#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    pipe(fd);

    pid_t id = fork();
    if (id == 0) {
        // 표준 출력을 파이프로 리다이렉션합니다.
        dup2(fd[1], STDOUT_FILENO);

        // 파이프를 닫습니다.
        close(fd[0]);
        close(fd[1]);

    // 파이프로 보낼 데이터를 준비합니다.
    const char *data = "Hello, CGI!";
    write(STDIN_FILENO, data, strlen(data));
    close(STDIN_FILENO); // 데이터를 모두 썼으므로 읽기 파이프를 닫습니다.
    
        // 쉘 스크립트를 실행합니다.
        execlp("bash", "bash", "test.sh", NULL);

        // execlp가 실패한 경우에만 실행됩니다.
        perror("execlp");
        exit(1);
    }

    // 부모 프로세스에서 파이프를 닫습니다.
    close(fd[0]);
    close(fd[1]);


    // 자식 프로세스가 종료될 때까지 기다립니다.
    waitpid(id, NULL, 0);

    return 0;
}
