#pragma once

#include "Util.hpp"
#include "AResponse.hpp"
#include "Server.hpp"
#include "Operation.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <sys/event.h>  // kqueue
#include <algorithm> // min
#include "enum.hpp"

class Chunked : public AResponse
{
	private:
		int _writeFd[2]; // parent(w) -> child(r)
		int _readFd[2]; // child(w) -> parent(r)
		pid_t _pid;	
		std::string _chunkedFilename;
	public:
		Chunked(Request* request, int kq);
		void createResponse();
		void uploadFile(const std::string& body);
		const std::string printResult(int fd, int kq);
		void endResponse();
		void childProcess();
		void execveCgi() const;
		//get
		pid_t getPid() const;
};
