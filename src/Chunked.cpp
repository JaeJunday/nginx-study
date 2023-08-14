#include "Client.hpp"

// NOTE_EXIT -> 프로세스 종료될때 이벤트 발생
// EV_SET(&event, pid, NOTE_EXIT, EV_ADD, 0, 0, nullptr);
// kevent(kq, &event, 1, NULL, 0, NULL);

// Chunked::Chunked(Request* request, int kq) : Client(kq), _pid(-2)
// {
// 	_request = request;
// }

void Client::chunkedCreateResponse()
{
	findLocationPath();
    std::vector<std::string> url = util::getToken(_request->getRequestUrl(), "/");
    if (url.size() >= 1)
        _chunkedFilename = url[url.size() - 1];
	pipe(_writeFd);
	pipe(_readFd);

	util::setEvent(_writeFd[1], _kq, EVFILT_WRITE);
	util::setEvent(_readFd[0], _kq, EVFILT_READ);
	_pid = fork();
	if (_pid == 0)
		childProcess();
	close(_writeFd[0]);
	close(_readFd[1]);
}

void Client::endResponse()
{
	close(_writeFd[1]);
	waitpid(_pid, NULL, 0);
	printResult(_readFd[0], _kq);
	close(_readFd[0]);
	// if (findLocationPath().empty())
	// 	throw 405;
	// checkLimitExcept();
}

void Client::childProcess()
{
	dup2(_writeFd[0], STDIN_FILENO);
	close(_writeFd[0]);
	close(_writeFd[1]);
	dup2(_readFd[1], STDOUT_FILENO);
	close(_readFd[0]);
	close(_readFd[1]);
	// 실행시킬 모듈을 골라서 스크립트 실행 파일 이름으로 실행시킴 
	execveCgi();
}

void Client::execveCgi() const
{
	std::string scriptPath;
	std::string engine;

	if (_chunkedFilename.find(".bla"))
	{
		scriptPath = "." + _request->getLocation()->_bla;
		engine = scriptPath;
	}
	else if (_chunkedFilename.find(".py"))
	{
		scriptPath = "." + _request->getLocation()->_cgiParam;
		engine = _request->getLocation()->_py;
	}
	std::cerr << RED << "testcode : " << "scriptPath :" << scriptPath << RESET << std::endl;
	std::cerr << RED << "testcode : " << "egine :" << engine << RESET << std::endl;

	// "./src/cgi/chunked_upload_cgi.py";  // 실행할 파이썬 스크립트의 경로
	char* const args[] = {const_cast<char*>("python3"), const_cast<char*>(scriptPath.c_str()), nullptr};
	setenv("FILENAME", _chunkedFilename.c_str(), true);
	setenv("CONTENT_TYPE", _request->getContentType().c_str(), true);
	extern char** environ;
	if (execve(engine.c_str(), args, environ) == -1) {
		perror("execve");  // 오류 처리
	}
}

void Client::uploadFile(size_t pipeSize)
{
	// 써야되는 사이즈 
	// _perfectBody.size(); // or pipeSize
	// endresponse
	size_t writeSize = std::min(_perfectBody.size(), pipeSize);
	writeSize = write(_request->getWriteFd(), _perfectBody.c_str() + _writeIndex, writeSize);
	if (writeSize > 0)
		_writeIndex += writeSize;
	if (_request->getBodyTotalSize() == _writeIndex)
		endResponse();
}

void Client::printResult()
{
	char tempBuffer[PIPESIZE];
	std::string readBuffer;
	memset(tempBuffer, 0, PIPESIZE);

	readSize = read(fd, tempBuffer, PIPESIZE);
	if (readSize < 0)
		break;
	readBuffer.append(tempBuffer, readSize);
	_buffer << readBuffer;
}

pid_t Client::getPid() const
{
	return _pid;
}