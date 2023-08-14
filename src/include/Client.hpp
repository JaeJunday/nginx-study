#pragma once

// #include "Operation.hpp"
#include "Request.hpp"
#include "enum.hpp"
#include "Util.hpp"
#include "Server.hpp"
#include "Color.hpp"
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

#define TIME_SIZE 40
#define PIPESIZE 42000

class Request;
class Server;

class Client
{
	private:
		Request*			_request;

		int					_writeFd[2]; // parent(w) -> child(r)
		int					_readFd[2]; // child(w) -> parent(r)
		pid_t				_pid;	
		std::string 		_chunkedFilename;
		std::string			_version;
		std::string			_stateCode;
		std::string			_reasonPhrase;
		std::time_t			_date;
		std::string 		_serverName;
		std::string			_contentType;
		size_t				_contentLength;
		std::stringstream	_buffer;
		int					_kq;
	// chunked를 위한
		std::string 		_perfectBody;
		int					_sendIndex;
	
		Client(const Client& src); 
		Client& operator=(Client const& rhs);
	public:
		Client(int kq);
		virtual ~Client();
		Client(Request* request);
		static std::string getDate();
		virtual void createResponse() =0;
		void stamp() const;
		const std::stringstream& getBuffer() const;
		std::string findLocationPath() const;
		void checkLimitExcept() const;
		std::string findContentType(const std::string& filePath);
		int getKq() const;
		Request* getRequest() const;
		int getStateCode() const;
		void getProcess();
		void postProcess();
		void deleteProcess();
		void errorProcess();		

	// get.cpp
        void getCreateResponse(); //override
        void openPath(const std::string& path);
        void fileProcess(const std::string& filePath, std::stringstream& body);
        void pushBuffer(std::stringstream& body);
		void autoIndexProcess(DIR* dirStream, std::stringstream& body);
	// post.cpp
		void postCreateResponse(); // override
        void childProcess(int *writeFd, int *readFd);
		void uploadFile(int fd, int kq);
        const std::string printResult(int fd, int kq);
	// delete.cpp
		void deleteCreateResponse(); // override
		void removeFile(std::string file) const;

	// chunked.cpp
		void chunkedCreateResponse();
		void uploadFile(const std::string& body);
		const std::string printResult(int fd, int kq);
		void endResponse();
		void childProcess();
		void execveCgi() const;
		//get.cpp
		pid_t getPid() const;
	// error.cpp
		void errorCreateResponse(); // override
		void makeErrorPage(int errnum);
		void pushErrorBuffer(std::string body, int errnum);
};
