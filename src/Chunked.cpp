#include "Client.hpp"
#include "include/enum.hpp"

// NOTE_EXIT -> 프로세스 종료될때 이벤트 발생
// EV_SET(&event, pid, NOTE_EXIT, EV_ADD, 0, 0, nullptr);
// kevent(kq, &event, 1, NULL, 0, NULL);

// Chunked::Chunked(Request* request, int kq) : Client(kq), _pid(-2)
// {
// 	_request = request;
// }

// void Client::chunkedCreateResponse()
// {
// 	findLocationPath();

// 	pipe(_writeFd);
// 	pipe(_readFd);

// 	util::addEvent(_writeFd[1], _kq, EVFILT_WRITE);
// 	util::addEvent(_readFd[0], _kq, EVFILT_READ);
// 	_pid = fork();
// 	if (_pid == 0)
// 		childProcess();
// 	close(_writeFd[0]);
// 	close(_readFd[1]);
// }

void Client::initCgi()
{
	std::vector<std::string> url = util::getToken(_request->getRequestUrl(), "/");
	if (url.size() >= 1)
		_request->setChunkedFilename(url[url.size() - 1]);
	pipe(_writeFd);
	pipe(_readFd);
	// addEvent(_readFd[0], EVFILT_READ);
	_pid = fork();
	if (_pid == 0)
		childProcess();
	close(_writeFd[0]);
	close(_readFd[1]);
}

// void Client::endResponse()
// {
	// close(_writeFd[1]);
	// waitpid(_pid, NULL, 0);
	// 	// std::cerr << RED << "print" << RESET << std::endl;
	// 	// printResult();
	// addEvent(_socketFd, EVFILT_WRITE);
	// _request->setEventState(EVFILT_WRITE);
	// _request->setChunkedState(chunk::END); // 업로드 데이터로 옮길부분
	// _request->setEventState(event::WRITE);
	// EV_SET(tevent, tevent->ident, EVFILT_WRITE, EV_ADD, 0, 0, client);
	// kevent(kq, tevent, 1, NULL, 0, NULL);
// if (findLocationPath().empty())
	// 	throw 405;
	// checkLimitExcept();
// }

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
	//std::cerr << RED << "uploadFile========" << RESET << std::endl;
std::cerr << BLUE << "pipe: " << pipeSize << RESET << std::endl;
std::cerr << BLUE << "wirteindex: " << _writeIndex << RESET << std::endl;
	std::string perfectBody = _request->getPerfectBody();
std::cerr << RED << "perfectBody : " << perfectBody << RESET << std::endl;
	size_t writeSize = std::min(perfectBody.size() - _writeIndex, pipeSize);
std::cerr << RED << "writeSize : " << writeSize << RESET << std::endl;
	writeSize = write(_writeFd[1], perfectBody.c_str() + _writeIndex, writeSize);
	if (writeSize < 0)
		return;
	if (writeSize >= 0)
		_writeIndex += writeSize;
std::cerr << RED << _request->getBodyTotalSize() << " : " << _writeIndex << RESET << std::endl;
	if (_request->getBodyTotalSize() == _writeIndex)
	{
std::cerr << RED << "end잇어용 😍" << RESET << std::endl;
		
		close(_writeFd[1]);
		addEvent(_readFd[0], EVFILT_READ);
	}
	// 엔드리스폰스 하는게 아니라 리드이벤트 걸어야함
	// cgi에서 메인 프로세스로 다 받았다는 플래그도 체크할 방법을 찾아야함
		// endResponse();
}

void Client::printResult()
{
	char tempBuffer[PIPESIZE];
	std::string readBuffer;
	memset(tempBuffer, 0, PIPESIZE);

	size_t readSize = read(_readFd[0], tempBuffer, PIPESIZE);
	std::cerr << RED << "readSize : " << readSize << RESET << std::endl;
	if (readSize < 0)
		return;
	if (readSize == 0) // end
	{
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
	_request->setBodyTotalSize(_request->getBuffer().size() - _request->getBodyStartIndex());
}