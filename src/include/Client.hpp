#pragma once

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
#include <cstdlib>
#include <algorithm> // min
#include <signal.h>
#include <sys/wait.h>
#include <sys/event.h>  // kqueue
#include <sys/types.h>
#include <sys/socket.h>

class Client
{
	private:
		Request*			_request;
		int					_socketFd;
		int					_writeFd[2];
		int					_readFd[2];
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
		std::string			_responseStr;
		int					_kq;
		int					_writeIndex;
		size_t				_sendIndex;
	
	public:
	// Client.cpp
		Client(Request* request, int kq, int socketFd);
		virtual ~Client();
		void handleResponse(const struct kevent &tevent);
		bool sendData(const struct kevent& tevent);
		void clearClient();
		void closePipeFd();
		void stamp() const;
	// Get.cpp
		void getProcess();
        void fileProcess(const std::string& filePath, std::stringstream& body);
        void pushBuffer(std::stringstream& body);
		void autoIndexProcess(DIR* dirStream, std::stringstream& body);
		bool isFilePy(const std::string& filePath);
		void getCgi();
		void getChildProcess();
	// Delete.cpp
		void deleteProcess();
		void removeFile(std::string file) const;
	// Post.cpp
		void postProcess();
		void writePipe(size_t pipeSize);
		void readPipe(size_t pipeSize);
		void childProcess();
		void execveCgi() const;
		void initCgi();
		void endChildProcess();
	// Error.cpp
		void errorProcess(int errnum);
		void pushErrorBuffer(std::string body, int errnum);
	// AddEvent.cpp
		void addSocketReadEvent();
		void addSocketWriteEvent();
		void addPipeReadEvent();
		void addPipeWriteEvent();
		void addProcessEvent();
		void addTimerEvent();
	// DeleteEvent.cpp
		void deleteReadEvent();
		void deleteWriteEvent();
		void deletePidEvent();
		void deleteTimerEvent();
		void resetTimerEvent();
	// ClientGetterAndSetter.cpp
		int							getWriteFd() 	const;
		int							getReadFd() 	const;
		int							getSocket() 	const;
		Request&					getReq() 		const;
		const std::stringstream&	getBuffer() 	const;
};
