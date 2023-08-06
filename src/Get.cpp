#include "Request.hpp"
#include "Server.hpp"
#include "Get.hpp"

Get::Get(Request* request, int kq) : AResponse(kq)
{
    _request = request;
}

Get::~Get()
{
    /* Destructor Implementation */
}

Get& Get::operator=(Get const& rhs)
{
    if (this != &rhs)
    {
        /* Assignment Operator Implementation */
    }
    return *this;
}	

void Get::createResponse()
{
    std::string path = findLocationPath();
	openPath(path);
}

void Get::openPath(const std::string& path)
{
	std::string relativePath = "." + path;
	std::cerr << "relative: " << relativePath << std::endl;
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
		else if (!_request->getServer().getRoot().empty())
		{
			relativePath += "/" + _request->getServer().getRoot();
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

void Get::fileProcess(const std::string& filePath, std::stringstream& body)
{
    std::ifstream		file;

	std::cerr << "filePath: " << filePath << std::endl;
	file.open(filePath.c_str());
	if (file.is_open() == true)
	{
		body << file.rdbuf();
		_contentType = 
		_contentLength += body.str().length();
		file.close();
	}
	else 
	{
		std::cerr << "not serverpath" << std::endl;
		std::cerr << "path: " << filePath << std::endl;
		//server errorcode
	}
}

void Get::autoIndexProcess(DIR* dirStream, std::stringstream& body)
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
	_contentType = "text/plane";
}

void Get::pushBuffer(std::stringstream& body)
{
	_buffer << _version << " " << _stateCode << " " << _reasonPhrase << "\r\n";
	_buffer << "Date: " << getDate() << "\r\n";
	_buffer << "Server: " << _serverName << "\r\n";
	_buffer << "Content-Type: " << _contentType << "\r\n";
	_buffer << "Content-Length: " << _contentLength << "\r\n\r\n";
	_buffer << body.str();
}