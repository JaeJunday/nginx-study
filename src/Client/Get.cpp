#include "Client.hpp"

void Client::handleGet()
{
	if (isFilePy(_request->getConvertRequestPath()))
	{
		handleGetCgi();
		return;
	}
	std::string relativePath = "." + _request->getConvertRequestPath();
	DIR *dirStream = opendir(relativePath.c_str());
	std::stringstream body;
	if (dirStream == NULL)
		handleFile(relativePath, body);
	else
		handleDir(relativePath, body, dirStream);
	pushBuffer(body);
	deleteSocketReadEvent();
	addSocketWriteEvent();
}

bool Client::isFilePy(const std::string& filePath)
{
	std::vector<std::string> files = util::getToken(filePath, "/");
	
	if (files[files.size() - 1].find(".py") != std::string::npos)
		return 1;
	else
		return 0;
}

void Client::handleGetCgi()
{
	if (pipe(_readFd) < 0)
		throw 500;
	_pid = fork();
	if (_pid < 0)
		throw 500;
	if (_pid == 0)
		handleGetChild();
	if (_pid > 0)
		addProcessEvent();
	fcntl(_readFd[0], F_SETFL, O_NONBLOCK);
	addPipeReadEvent();
	close(_readFd[1]);
}

void Client::handleGetChild()
{
	dup2(_readFd[1], STDOUT_FILENO);
	close(_readFd[0]);
	close(_readFd[1]);
	std::string engine = "." + _request->getConvertRequestPath();

	char* const args[] = {const_cast<char*>(engine.c_str()), NULL};
	if (execve(engine.c_str(), args, NULL) == -1)
		std::exit(EXIT_ERROR);
}

void Client::handleFile(const std::string& filePath, std::stringstream& body)
{
    std::ifstream		file;

	file.open(filePath.c_str());
	if (file.is_open() == false)
	{
		file.close();
		throw 404;
	}
	body << file.rdbuf();
	_contentType = util::findContentType(filePath);
	_contentLength += body.str().length();
	file.close();
}

void Client::handleDir(std::string& filePath, std::stringstream& body, DIR *dirStream)
{
	const Location* location = _request->getLocation();
	const Server* server = _request->getServer();
	
	if (!location->_tryFiles.empty() && location->_tryFiles != "/")
		filePath += "/" + location->_tryFiles;
	else if (!location->_index.empty())
		filePath += "/" + location->_index;
	else if (!server->getIndex().empty())
		filePath += "/" + server->getIndex();
	if (location->_autoindex == "on")
	{
		handleAutoIndex(dirStream, body);
		closedir(dirStream);
		return;
	}
	closedir(dirStream);
	handleFile(filePath, body);
}

void Client::handleAutoIndex(DIR* dirStream, std::stringstream& body)
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
