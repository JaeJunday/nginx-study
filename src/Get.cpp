#include "Request.hpp"
#include "Server.hpp"
#include "Get.hpp"

Get::Get() : AResponse()
{ 
}

Get::Get(Request* request) : AResponse()
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
	_buffer << _version << " " << _stateCode << " " << _reasonPhrase << "\r\n";
	_buffer << "Date: " << getDate() << "\r\n";
	_buffer << "Server: " << _serverName << "\r\n";
	_buffer << "Content-Type: " << _contentType << "\r\n";
    std::string path = findLocationPath();
	openPath(path);
}

void Get::fileProcess(const std::string& filePath)
{
    std::ifstream		file;
    std::stringstream	tmp;

	file.open(filePath.c_str());
	if (file.is_open() == true)
	{
		tmp << file.rdbuf();
		_contentLength += tmp.str().length();
		_buffer << "Content-Length: " << _contentLength << "\r\n\r\n";
		_buffer << tmp.str();
		file.close();
	}
	else 
	{
		std::cerr << "not serverpath" << std::endl;
		std::cerr << "path: " << filePath << std::endl;
		//server errorcode
	}
}

void Get::openPath(const std::string& path)
{
	std::string relativePath = "." + path;
	DIR *dir_stream = opendir(relativePath.c_str());
	
	if (dir_stream == NULL) 
		fileProcess(relativePath);
	else
	{
		if (!_request->getLocation()->_index.empty())
		{
			closedir(dir_stream);
			relativePath += "/" + _request->getLocation()->_index;
			fileProcess(relativePath);
		}
		else if (!_request->getServer().getRoot().empty())
		{
			closedir(dir_stream);
			relativePath += "/" + _request->getServer().getRoot();
			fileProcess(relativePath);
		}
		else if (_request->getLocation()->_autoindex == "on")
		{
			struct dirent *entry;
			while (entry != NULL)
			{
				entry = readdir(dir_stream);
				_buffer << std::string(entry->d_name) + "\n";
			}
			_contentLength = _buffer.str().length();
			closedir(dir_stream);
		}
		else
		{
			closedir(dir_stream);
			std::cerr << "no file" << std::endl;
			// 보여줄게 없다는 메시지를 띄운다(state errorcode는 200으로 처리한다. 왜냐하면 폴더는 있기 때문에).
		}
	}
}
