#include "Client.hpp"

// NOTE_EXIT -> 프로세스 종료될때 이벤트 발생
// EV_SET(&event, pid, NOTE_EXIT, EV_ADD, 0, 0, nullptr);
// kevent(kq, &event, 1, NULL, 0, NULL);

// Chunked::Chunked(Request* request, int kq) : Client(kq), _pid(-2)
// {
// 	_request = request;
// }


void Client::initCgi()
{
	std::cerr << RED << "_request->getRequestUrl() : " << _request->getRequestUrl() << RESET << std::endl;
	std::vector<std::string> url = util::getToken(_request->getRequestUrl(), "/");
	if (url.size() >= 1)
		_request->setChunkedFilename(url[url.size() - 1]);
	pipe(_writeFd);
	pipe(_readFd);
	addEvent(_readFd[0], EVFILT_READ);
	_pid = fork();
	if (_pid == 0)
		childProcess();
// std::cerr << BLUE << "pid: " << _pid << RESET << std::endl;
	close(_writeFd[0]);
	close(_readFd[1]);
}

void Client::childProcess()
{
std::cerr << PURPLE << "child process()" << RESET << std::endl;
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

//std::cerr << RED << "location: " << _request->getLocation()->_path << RESET << std::endl;
//std::cerr << RED << "_chunkedFilename : " << _request->getChunkedFilename() << RESET << std::endl;

	if (_request->getChunkedFilename().find(".bla") != std::string::npos)
	{
		scriptPath = "." + _request->getLocation()->_bla;
		engine = scriptPath;
	}
	else
	{
		scriptPath = "." + _request->getLocation()->_py;
		engine = scriptPath;
	}

	// std::cerr << RED << "testcode : " << "scriptPath :" << scriptPath << RESET << std::endl;
	// std::cerr << RED << "testcode : " << "egine :" << engine << RESET << std::endl;

	// std::string argFirst;
	// size_t lastSlashPos = scriptPath.find_last_of('/');
    // if (lastSlashPos != std::string::npos) {
    //     argFirst = scriptPath.substr(lastSlashPos + 1);
    // }
	// "./src/cgi/chunked_upload_cgi.py";  // 실행할 파이썬 스크립트의 경로
	char* const args[] = {const_cast<char*>(engine.c_str()), NULL}; 

	setenv("FILENAME", _request->getChunkedFilename().c_str(), true);
	setenv("CONTENT_TYPE", _request->getContentType().c_str(), true);
	setenv("REQUEST_METHOD", _request->getMethod().c_str(), true);
	setenv("SERVER_PROTOCOL", "HTTP/1.1", true);
	setenv("PATH_INFO", scriptPath.c_str(), true);
	setenv("DOCUMENT_ROOT", _convertRequestPath.c_str(), true);
	if (_request->getSecretHeader().empty() == false)
		setenv("X_SECRET_HEADER_FOR_TEST", _request->getSecretHeader().c_str(), true);
	extern char** environ;
	if (execve(engine.c_str(), args, environ) == -1) {
		perror("execve");  // 오류 처리
		exit(1);
	}
}

void Client::uploadFile(size_t pipeSize)
{
	std::string perfectBody = _request->getPerfectBody();
	// if (_writeIndex == 0)

	int writeSize = std::min(perfectBody.size() - _writeIndex, pipeSize);
	writeSize = write(_writeFd[1], perfectBody.c_str() + _writeIndex, writeSize);
	if (writeSize <= 0)
		throw 500;
	_writeIndex += writeSize;
// std::cerr << _request->getBodyTotalSize() <<" ♡ "<< _writeIndex << "\n";
	if (_request->getBodyTotalSize() == _writeIndex)
	{	
		// addEvent(_readFd[0], EVFILT_READ);
// std::cerr << PURPLE << "uploadFile" << RESET << std::endl;
		close(_writeFd[1]);
	}
}

void Client::printResult(size_t pipeSize)
{
	char tempBuffer[pipeSize];
	std::string readBuffer;
	memset(tempBuffer, 0, pipeSize);

	std::cerr << RED << "pipeSize : " << pipeSize << RESET << std::endl;
	ssize_t readSize = read(_readFd[0], tempBuffer, pipeSize);
	if (readSize < 0)
		throw 500;
	if (readSize == 0) // end
	{
		// std::cerr << BLUE << _responseBuffer.str() << RESET << std::endl; // 어떻게 넘어왔는지 확인
		if (_request->getChunkedFilename().find(".bla") != std::string::npos) {
			std::string msg = _responseBuffer.str();
			size_t cgiHeaderSize = msg.find("\r\n") + 2;
			size_t cgiBodySize = msg.size() - (msg.find("\r\n\r\n") + 4);
			_responseBuffer.str("");
			_responseBuffer << "HTTP/1.1 200 OK\r\n";
			_responseBuffer << "Content-Length: " << cgiBodySize << "\r\n";
// std::cerr << B_RED << "testcode " << "_responseBuffer : head >>> " << _responseBuffer.str() << RESET << std::endl;
// std::cerr << PURPLE << "testcode " << "msg : head >>> " << msg.substr(0, msg.find("\r\n\r\n") + 4) << RESET << std::endl;
			_responseBuffer << msg.substr(cgiHeaderSize, msg.size() - cgiHeaderSize);
			// _responseBuffer << "test"
			//  std::cerr << PURPLE << "testcode " << "_responseBuffer : head >>> " << _responseBuffer.str().substr(0, _responseBuffer.str().find("\r\n\r\n") + 4) << RESET << std::endl;
			// _responseBuffer << "\r\n\r\n";
			//body 제대로 만들어졌는지 확인
			// std::cout << _responseBuffer.str() << std::endl;
		}
		close(_readFd[0]);	
		waitpid(_pid, NULL, 0);
		deleteEvent();	
		addEvent(_socketFd, EVFILT_WRITE); // socket
		_request->setEventState(EVFILT_WRITE);
	}
	readBuffer.append(tempBuffer, readSize);
	_responseBuffer << readBuffer;
}

pid_t Client::getPid() const
{
	return _pid;
}

void Client::postProcess()
{
	_request->setPerfectBody(_request->getBuffer().c_str() + _request->getBodyStartIndex());
	_request->setBodyTotalSize(_request->getPerfectBody().size());
	addEvent(_writeFd[1], EVFILT_WRITE);
}


//secret header set env해줘야함
// 0은 가능 100까지만 가능하게 제한 pos