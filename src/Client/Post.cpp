#include "Client.hpp"

void Client::handlePost()
{
	std::string requestBody = _request->getRequestBuffer().substr(_request->getBodyStartIndex(), util::stoui(_request->getContentLength()));

	_request->setRequestBody(requestBody);
	_request->setBodyTotalSize(requestBody.size());
	if (_request->getLocation()->_clientMaxBodySize.empty() == false &&
		static_cast<uint32_t>(_request->getBodyTotalSize()) > util::stoui(_request->getLocation()->_clientMaxBodySize))
		throw 413;
	addPipeWriteEvent();
	_request->setChunkedEnd(true);
}

void Client::handlePostCgi()
{
	if (pipe(_writeFd) < 0 || pipe(_readFd) < 0)
		throw 500;
	_pid = fork();
	if (_pid < 0)
		throw 500;
	if (_pid == 0)
	{
		handlePostDup2();
		handleExeCgi();
	}
	if (_pid > 0)
		addProcessEvent();
	fcntl(_writeFd[1], F_SETFL, O_NONBLOCK);
	fcntl(_readFd[0], F_SETFL, O_NONBLOCK);
	addPipeReadEvent();
	addPipeWriteEvent();
	close(_writeFd[0]);
	close(_readFd[1]);
}

void Client::handlePostDup2()
{
	dup2(_writeFd[0], STDIN_FILENO);
	close(_writeFd[0]);
	close(_writeFd[1]);
	dup2(_readFd[1], STDOUT_FILENO);
	close(_readFd[0]);
	close(_readFd[1]);
}

void Client::handleExeCgi() const
{
	std::string engine = "." + _request->getLocation()->_py;
	if (_request->getConvertRequestPath().find(".bla") != std::string::npos)
		engine = "." + _request->getLocation()->_bla;
	char* const args[] = {const_cast<char*>(engine.c_str()), NULL};
	
	setenv("BOUNDARY", _request->getBoundary().c_str(), true);
	setenv("DOCUMENT_ROOT", _request->getConvertRequestPath().c_str(), true);
	setenv("REQUEST_METHOD", _request->getMethod().c_str(), true);
	setenv("PATH_INFO", engine.c_str(), true);
	setenv("SERVER_PROTOCOL", "HTTP/1.1", true);
	setenv("CONTENT_LENGTH", _request->getContentLength().c_str(), true);
	setenv("CONTENT_TYPE", _request->getContentType().c_str(), true);
	setenv("HTTP_TRANSFER_ENCODING", _request->getTransferEncoding().c_str(), true);
	setenv("HTTP_X_SECRET_HEADER_FOR_TEST", _request->getSecretHeader().c_str(), true);
	extern char** environ;
	if (execve(engine.c_str(), args, environ) == -1) 
		std::exit(EXIT_ERROR);
}

void Client::writePipe(size_t pipeSize)
{
	std::string& requestBody = _request->getRequestBody();
	size_t currentWriteSize = std::min(requestBody.size() - _writeIndex, pipeSize);
	ssize_t writeSize = write(_writeFd[1], requestBody.c_str() + _writeIndex, currentWriteSize);
	if (writeSize < 0)
		throw 500;
	_writeIndex += writeSize;
	if (writeSize == 0)
	{
		if (_request->getBodyTotalSize() == _writeIndex && _request->getChunkedEnd() == true)
			close(_writeFd[1]);
	}
}

void Client::readPipe(size_t pipeSize)
{
	char tempBuffer[pipeSize];
	std::string readBuffer;
	memset(tempBuffer, 0, pipeSize);

	ssize_t readSize = read(_readFd[0], tempBuffer, pipeSize);
	if (readSize < 0)
		throw 500;
	if (readSize == 0)
	{
		std::string msg = _responseBuffer.str();
		size_t cgiHeaderSize = msg.find("\r\n") + 2;
		size_t cgiBodySize = msg.size() - (msg.find("\r\n\r\n") + 4);
		_responseBuffer.str("");
		_responseBuffer << "HTTP/1.1 200 OK\r\n";
		_responseBuffer << "Content-Length: " << cgiBodySize << "\r\n";
		_responseBuffer << msg.substr(cgiHeaderSize, msg.size() - cgiHeaderSize);
		_responseStr = _responseBuffer.str();
		close(_readFd[0]);
	}
	readBuffer.append(tempBuffer, readSize);
	_responseBuffer << readBuffer;
}