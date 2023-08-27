#include "Client.hpp"
/*
	[Done] - kyeonkim
	- 평가표에 You have to test the CGI with the "GET" and "POST" methods. 가 있으므로
	~.py 로 GET 요청이 들어올 경우에도 ~.py를 실행한 결과값을 send 해줘야한다.
	- You can use a script containing an infinite loop or an error. 
	이런 항목도 있으므로 while.py 하나 만들어서 무한루프 시키는 파일을 하나 만들어둬야한다.
*/
// void Client::getProcess()
// {
//     // std::string path = findLocationPath();
// 	// checkLmitExcept();
// 	openPath(_request->getConvertRequestPath());
// }

void Client::getProcess()
{
	std::string relativePath = "." + _request->getConvertRequestPath();
	DIR *dirStream = opendir(relativePath.c_str());
	std::stringstream body;
	
	if (dirStream == NULL) 
		fileProcess(relativePath, body);
	else
	{
		if (!_request->getLocation()->_tryFiles.empty())
		{
			if (_request->getLocation()->_tryFiles != "/")
				relativePath += "/" + _request->getLocation()->_tryFiles;
			fileProcess(relativePath, body);
		}
		else if (!_request->getLocation()->_index.empty())
		{
			relativePath += "/" + _request->getLocation()->_index;
			fileProcess(relativePath, body);
		}
		else if (!_request->getServer()->getRoot().empty())
		{
			relativePath += "/" + _request->getServer()->getRoot();
			fileProcess(relativePath, body);
		}
		else if (_request->getLocation()->_autoindex == "on")
			autoIndexProcess(dirStream, body);
		closedir(dirStream);
	}
	pushBuffer(body);
}

bool Client::isFilePy(const std::string& filePath)
{
	std::vector<std::string> files = util::getToken(filePath, "/");
	
	if (files[files.size() - 1].find(".py") != std::string::npos)
		return 1;
	else
		return 0;
}

void Client::getChildProcess()
{
	dup2(_readFd[1], STDOUT_FILENO);
	close(_readFd[0]);
	close(_readFd[1]);
	std::string engine = "." + _request->getConvertRequestPath();

	char* const args[] = {const_cast<char*>(engine.c_str()), NULL};
	// 실행시킬 모듈을 골라서 스크립트 실행 파일 이름으로 실행시킴 
	if (execve(engine.c_str(), args, NULL) == -1)
	{
		perror("execve");  // 오류 처리
		exit(1);
	}
}

/*	kyeonkim
	@des get도 cgi 처리가 되어야한다. 현재 /src/pages 폴더에 read.py를 추가해놨다.
	http://0.0.0.0:80/read.py 하면 실행시켜서 보여준다.
	post에서 사용한 걸 그대로 쓰고 싶었지만 get은 writeFd에 이벤트를 걸 필요가 없으므로 분리시켜서 구현
*/
void Client::getCgi()
{
	if (pipe(_readFd) < 0)
		throw 500;
	_pid = fork();
	if (_pid < 0)
		throw 500;
	if (_pid == 0)
		getChildProcess();
	if (_pid > 0)
		addProcessEvent();
	fcntl(_readFd[0], F_SETFL, O_NONBLOCK);
	addPipeReadEvent();
	close(_readFd[1]);
}

void Client::fileProcess(const std::string& filePath, std::stringstream& body)
{
    std::ifstream		file;

	file.open(filePath.c_str());
	if (file.is_open() == true)
	{
		body << file.rdbuf();
		_contentType = util::findContentType(filePath);
		_contentLength += body.str().length();
		file.close();
	}
	else 
	{
		std::cerr << "server errorcode : not serverpath" << std::endl;
		throw 404;
	}
}

void Client::autoIndexProcess(DIR* dirStream, std::stringstream& body)
{
	struct dirent *entry;
	while (true)
	{
		entry = readdir(dirStream);
		if (entry == NULL)
			break;
		body << std::string(entry->d_name) << "\n";
	}
	_contentLength = body.str().length();
	_contentType = "text/plain";
}

void Client::pushBuffer(std::stringstream& body)
{
	_responseBuffer << _version << " " << _stateCode << " " << _reasonPhrase << "\r\n";
	_responseBuffer << "Date: " << util::getDate() << "\r\n";
	_responseBuffer << "Server: " << _serverName << "\r\n";
	_responseBuffer << "Content-Type: " << _contentType << "\r\n";
	_responseBuffer << "Content-Length: " << _contentLength << "\r\n\r\n";
	_responseBuffer << body.str();
	_responseStr = _responseBuffer.str();
}