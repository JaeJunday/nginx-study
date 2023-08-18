#pragma once

// #include "Operation.hpp"

#include "Request.hpp"
#include "enum.hpp"
#include "Util.hpp"
#include "Server.hpp"
#include "Color.hpp"
#include <cstdint>
#include <iostream> 
#include <string> 
#include <ctime>
#include <sstream> 
#include <fstream> 
#include <vector>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <sys/event.h>  // kqueue
#include <algorithm> // min
#include <signal.h>
#include <sys/types.h>

#define TIME_SIZE 40
#define PIPESIZE 42000

class Client
{
	private:
		Request*			_request;
		int					_socketFd;
		int					_writeFd[2]; // parent(w) -> child(r)
		int					_readFd[2]; // child(w) -> parent(r)
		pid_t				_pid;
		std::string 		_chunkedFilename;
		std::string			_version;
		int					_stateCode;
		std::string			_reasonPhrase;
		std::time_t			_date;
		std::string 		_serverName;
		std::string			_contentType;
		size_t				_contentLength;
		std::stringstream	_responseBuffer;
		int					_kq;
		int					_writeIndex;
		std::string			_convertRequestPath;
		size_t				_sendIndex;
	
		Client(const Client& src); 
		Client& operator=(const Client& rhs);
	public:
		virtual ~Client();
		Client(Request* request, int kq, int socketFd);
		static std::string getDate();
		void stamp() const;
		std::string findLocationPath() const;
		void checkLimitExcept() const;
		std::string findContentType(const std::string& filePath);

		void getProcess();
		void postProcess();
		void deleteProcess();
		void errorProcess(int errnum);


		bool sendData(struct kevent& tevent);
	// get.cpp
        void openPath(const std::string& path);
        void fileProcess(const std::string& filePath, std::stringstream& body);
        void pushBuffer(std::stringstream& body);
		void autoIndexProcess(DIR* dirStream, std::stringstream& body);
	// delete.cpp
		void removeFile(std::string file) const;
	// post.cpp
		void uploadFile(size_t pipeSize);
		void printResult(size_t pipeSize);
		void childProcess();
		void execveCgi() const;
		void initCgi();
		//get.cpp
		pid_t getPid() const;
	// error.cpp
		void pushErrorBuffer(std::string body, int errnum);
	// setevent
		void addEvent(int fd, int filter);
		void deleteReadEvent();
		void deleteWriteEvent();
		
		void deletePidEvent();

	// get
		int getWriteFd() const;
		int getReadFd() const;
		int getKq() const;
		Request& getReq() const;
		int getSocket() const;
		int getStateCode() const;
		const std::stringstream& getBuffer() const;
	// clear
		void clearClient();
		void handleRequest(struct kevent* tevent, char* buffer);
		void handleResponse(struct kevent *tevent);
		void endChildProcess();
};
