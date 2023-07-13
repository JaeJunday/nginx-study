#include <iostream>     // cout
#include <sys/types.h>  // socket, bind
#include <sys/event.h>  // kqueue
#include <sys/socket.h> // socket
#include <netinet/in.h> // sockaddr_in
#include <unistd.h>     // read, write
#include <arpa/inet.h>  // inet
#include <fstream>      // ifstream
#include <signal.h>     // signal
#include <fcntl.h>      // fcntl
#define MAX_EVENTS 10
#define BACKLOG 5

int server_fd, client_fd;
int kq, nev;

void handler(int sig)
{
	std::cout << sig << std::endl;
	close(client_fd);
    close(server_fd);
    close(kq);
}

int main() {
    // int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    struct kevent event, events[MAX_EVENTS];
    // int kq, nev;

	signal(SIGINT, &handler);
    // 서버 소켓 생성
    server_fd = socket(AF_INET, SOCK_STREAM,  0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    fcntl(server_fd, F_SETFL, O_NONBLOCK);

	int optval = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    fcntl(server_fd, F_SETFL, O_NONBLOCK);
    // 서버 소켓 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(12345);
    // 서버 소켓에 바인딩
    if (bind(server_fd, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
        perror("Socket bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    // 연결 대기 상태로 진입
    if (listen(server_fd, 5) == -1) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    // kqueue 객체 생성
    kq = kqueue();
    if (kq == -1) {
        perror("Kqueue creation failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    // 서버 소켓을 kqueue에 등록
    EV_SET(&event, server_fd, EVFILT_READ, EV_ADD, 0, 0, nullptr); // 이벤트 SET
    if (kevent(kq, &event, 1, nullptr, 0, nullptr) == -1) {
        perror("Kqueue event registration failed");
        close(server_fd);
        close(kq);
        exit(EXIT_FAILURE);
    }
    std::cout << "Server listening on port 12345" << std::endl;
    while (true) {
        // 이벤트 감지
        nev = kevent(kq, nullptr, 0, events, MAX_EVENTS, nullptr);
        if (nev == -1) {
            perror("Kqueue event detection failed");
            close(server_fd);
            close(kq);
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < nev; i++) {
            if (events[i].ident == (uintptr_t)server_fd) {
                // 새로운 연결 요청 수락
                client_len = sizeof(client_addr);
                client_fd = accept(server_fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
                if (client_fd == -1) {
                    perror("Accept failed");
                    close(server_fd);
                    close(kq);
                    exit(EXIT_FAILURE);
                }
                std::cout << "New client connected: " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;
                // 새로운 클라이언트 소켓을 kqueue에 등록
                EV_SET(&event, client_fd, EVFILT_READ, EV_ADD, 0, 0, nullptr);
                if (kevent(kq, &event, 1, nullptr, 0, nullptr) == -1) {
                    perror("Kqueue event registration failed");
                    close(server_fd);
                    close(client_fd);
                    close(kq);
                    exit(EXIT_FAILURE);
                }
            } else {
                // 클라이언트로부터의 데이터 읽기
                char buffer[1024];
                // ssize_t bytes_read = read(events[i].ident, buffer, sizeof(buffer));
                ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);
				std::cout << buffer << std::endl;
				std::cout << bytes_read << std::endl;
                if (bytes_read == -1) {
                    perror("Read failed");
                    close(server_fd);
                    close(kq);
                    exit(EXIT_FAILURE);
                }
                if (bytes_read == 0) {
                    // 클라이언트 연결 종료
                    std::cout << "Client disconnected" << std::endl;
                    close(events[i].ident);
                } else {
                    // 받은 데이터 출력
                    std::string buf;
                    std::string result;
                    std::ifstream file;
                    file.open("index.html", std::ifstream::in);
					// std::string a = "HTTP/1.1 200 OK\r\nContent-Length:5\r\n\r\nHELLO";
					// "HTTP/1.1 200 OK\r\nContent-Length:5\r\n\r\nHELLO";
                    result += "HTTP/1.1 200 OK\r\n";
                    result += "Content-Length:254\r\n";
                    result += "Content-Type: text/html\r\n";
					result += "Connection: close\r\n";
                    result += "Server: MyServer\r\n";
                    result += "\r\n";
                    while(true)
                    {
                        std::getline(file, buf);
						if (file.eof())
							break;
                        result += buf;
                    }
                    file.close();
					// std::cout << result << std::endl;
                    // bytes_read = ;
        			// send(client_fd, result.c_str(), result.length(), 0);
        			send(client_fd, result.c_str(), result.length(), 0);
                    // std::cout << "Received data from client: " << std::string(buffer, bytes_read) << std::endl;
                }
            }
        }
    }
    // 아 ~ 아 ~
	close(client_fd);
    close(server_fd);
    close(kq);
    return 0;
}