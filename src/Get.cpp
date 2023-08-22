#include "Client.hpp"

// Get::Get(Request* request, int kq) : AResponse(kq)
// {
//     _request = request;
// }

// Get::~Get()
// {
//     /* Destructor Implementation */
// }

// Get& Get::operator=(Get const& rhs)
// {
//     if (this != &rhs)
//     {
//         /* Assignment Operator Implementation */
//     }
//     return *this;
// }

/*
	[Feat] - kyeonkim
	- 평가표에 You have to test the CGI with the "GET" and "POST" methods. 가 있으므로
	~.py 로 GET 요청이 들어올 경우에도 ~.py를 실행한 결과값을 send 해줘야한다.
	- You can use a script containing an infinite loop or an error. 이런 항목도 있으므로
	while.py 하나 만들어서 무한루프 시키는 파일을 하나 만들어둬야한다.
*/

void Client::getProcess()
{
    std::string path = findLocationPath();
	checkLimitExcept();
	openPath(path);
}

void Client::openPath(const std::string& path)
{
	std::string relativePath = "." + path;
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
		else
		{
			std::cerr << "no file" << std::endl;
			// 보여줄게 없다는 메시지를 띄운다(state errorcode는 200으로 처리한다. 왜냐하면 폴더는 있기 때문에).
		}
		closedir(dirStream);
	}
	pushBuffer(body);
}

void Client::fileProcess(const std::string& filePath, std::stringstream& body)
{
    std::ifstream		file;

	file.open(filePath.c_str());
	if (file.is_open() == true)
	{
		body << file.rdbuf();
		_contentType = findContentType(filePath);
		_contentLength += body.str().length();
		file.close();
	}
	else 
	{
		std::cerr << "not serverpath" << std::endl;
		// std::cerr << "path: " << filePath << std::endl;
		throw 404;
		//server errorcode
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
		_contentLength += body.str().length();
	}
	_contentType = "text/plain";
}

void Client::pushBuffer(std::stringstream& body)
{
	_responseBuffer << _version << " " << _stateCode << " " << _reasonPhrase << "\r\n";
	_responseBuffer << "Date: " << getDate() << "\r\n";
	_responseBuffer << "Server: " << _serverName << "\r\n";
	_responseBuffer << "Content-Type: " << _contentType << "\r\n";
	_responseBuffer << "Content-Length: " << _contentLength << "\r\n\r\n";
	_responseBuffer << body.str();
	_responseStr = _responseBuffer.str();
}