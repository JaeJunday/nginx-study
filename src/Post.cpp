#include "Client.hpp"

void Client::initCgi()
{
	std::cerr << RED << "func: initCgi()" << RESET << std::endl;

	if (pipe(_writeFd) < 0 || pipe(_readFd) < 0)
		throw 500;

	_pid = fork();
	if (_pid < 0)
		throw 500;
	if (_pid == 0)
		childProcess();
	if (_pid > 0)
		addEvent(_pid, EVFILT_PROC);
	fcntl(_writeFd[1], F_SETFL, O_NONBLOCK);
	fcntl(_readFd[0], F_SETFL, O_NONBLOCK);
	addEvent(_readFd[0], EVFILT_READ);
	addEvent(_writeFd[1], EVFILT_WRITE);
	close(_writeFd[0]);
	close(_readFd[1]);
}

void Client::childProcess()
{
// std::cerr << PURPLE << "child process()" << RESET << std::endl;
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
	std::string engine = "." + _request->getLocation()->_py;

	if (_convertRequestPath.find(".bla") != std::string::npos)
		engine = "." + _request->getLocation()->_bla;
	char* const args[] = {const_cast<char*>(engine.c_str()), NULL};
	setenv("BOUNDARY", _request->getBoundary().c_str(), true);
	setenv("DOCUMENT_ROOT", _convertRequestPath.c_str(), true);
	setenv("REQUEST_METHOD", _request->getMethod().c_str(), true);
	setenv("PATH_INFO", engine.c_str(), true);
	setenv("SERVER_PROTOCOL", "HTTP/1.1", true);
	setenv("CONTENT_LENGTH", _request->getContentLength().c_str(), true);
	setenv("CONTENT_TYPE", _request->getContentType().c_str(), true);
	setenv("HTTP_TRANSFER_ENCODING", _request->getTransferEncoding().c_str(), true);
	setenv("HTTP_X_SECRET_HEADER_FOR_TEST", _request->getSecretHeader().c_str(), true);
	extern char** environ;
	if (execve(engine.c_str(), args, environ) == -1) {
		perror("execve");  // 오류 처리
		exit(1);
	}
}

void Client::uploadFile(size_t pipeSize)
{
	std::string perfectBody = _request->getPerfectBody();
	size_t currentWriteSize = std::min(perfectBody.size() - _writeIndex, pipeSize);
	ssize_t writeSize = write(_writeFd[1], perfectBody.c_str() + _writeIndex, currentWriteSize);
	if (writeSize < 0)
	{
		// std::cerr << B_RED << "testcode " << "writeSize error" << RESET << std::endl;
		// std::cerr << B_RED << "testcode " << strerror(errno) << RESET << std::endl;
		// return;
		throw 500;
	}
	_writeIndex += writeSize;
std::cerr << B_BG_CYAN <<  "fd: " << _socketFd << " : " << _request->getBodyTotalSize() <<" ♡ "<< _writeIndex << RESET << std::endl;
std::cerr << B_BG_CYAN <<  "_request->getChunkedEnd() : " << _request->getChunkedEnd() << RESET << std::endl;
	if (_request->getBodyTotalSize() == _writeIndex && _request->getChunkedEnd() == true)
	{
		close(_writeFd[1]);
		// _writeFd[1] = -2;
	}
}

void Client::printResult(size_t pipeSize)
{
	char tempBuffer[pipeSize];
	std::string readBuffer;
	memset(tempBuffer, 0, pipeSize);

	ssize_t readSize = read(_readFd[0], tempBuffer, pipeSize);
	if (readSize < 0)
	{
		// std::cerr << B_RED << "testcode " << "readSize error" << RESET << std::endl;
		// std::cerr << B_RED << "testcode " << strerror(errno) << RESET << std::endl;
		// return;
		throw 500;
	}
std::cerr << B_BG_PURPLE <<"fd: " << _socketFd << " : " << _request->getBodyTotalSize() <<" ♡ "<< readSize << RESET<< std::endl;
	if (readSize == 0)
	{
		std::cerr << RED << "fd: " << _socketFd << " read pipe end" << RESET << std::endl;
		std::string msg = _responseBuffer.str();
		size_t cgiHeaderSize = msg.find("\r\n") + 2;
		size_t cgiBodySize = msg.size() - (msg.find("\r\n\r\n") + 4);
		_responseBuffer.str("");
		_responseBuffer << "HTTP/1.1 200 OK\r\n";
		_responseBuffer << "Content-Length: " << cgiBodySize << "\r\n";
		_responseBuffer << msg.substr(cgiHeaderSize, msg.size() - cgiHeaderSize);
		close(_readFd[0]);
		// _readFd[0] = -2;
	}
	readBuffer.append(tempBuffer, readSize);
	_responseBuffer << readBuffer;
}

void Client::endChildProcess()
{
std::cerr << RED << "endchild process()" << RESET << std::endl;
	waitpid(_pid, NULL, 0);
	// 반환값 받아서 확인 
	deleteReadEvent();
	addEvent(_socketFd, EVFILT_WRITE); // socket
	_request->setEventState(EVFILT_WRITE);	
	_pid = -2;
}

pid_t Client::getPid() const
{
	return _pid;
}

void Client::postProcess()
{
	std::string body = _request->getBuffer().substr(_request->getBodyStartIndex(), util::stoui(_request->getContentLength()));
	_request->setPerfectBody(body); // _perfectbody > _requestBody 로 바꾸고 싶음 - kyeonkim
	_request->setBodyTotalSize(body.size());

	if (_request->getLocation()->_clientMaxBodySize.empty() == false)
	{
		std::cerr << BLUE << "maxbodysize: " <<  util::stoui(_request->getLocation()->_clientMaxBodySize) << RESET << std::endl;
		std::cerr << BLUE << "bodySize: " << _request->getBodyTotalSize() << std::endl;
		if (_request->getBodyTotalSize() > util::stoui(_request->getLocation()->_clientMaxBodySize))
			throw 413;
	}
	addEvent(_writeFd[1], EVFILT_WRITE);
}

//secret header set env해줘야함
// 0은 가능 100까지만 가능하게 제한 pos