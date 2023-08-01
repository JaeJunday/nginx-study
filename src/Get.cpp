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

/*
// 형식
<버전> <상태 코드> <사유 구절>
<헤더>

<본문>

// ex
HTTP/1.1 200 OK
Content-type: text/html Context-length: 42
...

Body line1
Body line2
...
HTTP/1.1 200 OK
Date: Tue, 20 Jul 2023 12:34:56 GMT
Server: Apache/2.4.38 (Unix)
Content-Type: text/html; charset=UTF-8
Content-Length: 1024
*/

// PostData 를 어디서 넣어주는가?? - semikim
void Get::createResponseHeader(std::vector<Server> servers)
{
	_buffer << _version << " " << _stateCode << " " << _reasonPhrase << "\r\n";
	_buffer << "Date: " << getDate() << "\r\n";
	_buffer << "Server: " << _serverName << "\r\n";
	_buffer << "Content-Type: " << _contentType << "\r\n";
    std::ifstream	file;
    std::stringstream tmp;
    // 경로에 따라서 맞는 경로의 파일을 오픈
    std::string found = findLocationPath(servers);
	std::cerr << "찾은경로" << found << std::endl;

    if (found.length() > 0 || _request->getRequestUrl() == "/")
	{
		int serverSocket = _request->getServerSocket();
		Server server;
		for (int i = 0; i < servers.size(); ++i) 
		{
			if (servers[i].getSocket() == serverSocket)
				server = servers[i];
		}

		Location location;
		std::string filename;
		for (int i = 0; i < server.getLocationSize(); ++i) 
		{
			location = server.getLocation(i);
			filename = findFilename("./src/pages" + location._path);
			file.open(filename);
			if (file.is_open() == false)
				continue;
			tmp << file.rdbuf();
			_contentLength += tmp.str().length();
			file.close();	
		}	
	}
    else
    {
        file.open("src/pages/error/404.html");
		if (file.is_open() == false)
            throw std::runtime_error("Error: file not found error");
        tmp << file.rdbuf();
        _contentLength = tmp.str().length();
        file.close();
    }
	_buffer << "Content-Length: " << _contentLength << "\r\n\r\n";
}


std::string Get::findFilename(const std::string& filePath) const
{
	// 디렉 토리를 먼저 오픈
	
	std::string result;
	const char *dirname = filePath.c_str(); 
	DIR *dir_stream = opendir(dirname);
	if (dir_stream == NULL) {
		return std::string("");
	}
	struct dirent *entry = readdir(dir_stream);
	if (entry != NULL) 
	{
		closedir(dir_stream);
		return  std::string(entry->d_name);
	}
	closedir(dir_stream);
	return result;
}

std::string Get::findLocationPath(std::vector<Server> servers) const
{
    std::vector<std::string> path = util::getToken(const_cast<std::string&>(_request->getRequestUrl()), "\\");
    std::string tmp;
	int i = 0;
	while (i < servers.size()) 
	{
		if (servers[i].getSocket() == _request->getServerSocket())	
			break;
		++i;
	}
	if (i == servers.size())
		return tmp;

	std::vector<Location> locations = servers[i].getLocations();
    for (int i = path.size() - 1; i > -1; --i) {
        if (i != path.size() - 1)
            tmp += "\\";
        tmp += path[i];   
		for (int j = 0; j < locations.size(); ++j)
		{
			if (locations[i]._path == tmp)
			return tmp;
		}
    }
	return std::string("");
}

void Get::createResponseMain()
{
    std::ifstream	file;

    if (_request->getRequestUrl() == "/")
	{
		file.open("src/pages/hello.html");
		if (file.is_open() == false)
            throw std::runtime_error("Error: file not found error");
        _buffer << file.rdbuf();
        file.close();
	}
    else
    {
		file.open("src/pages/hello.html");
		// file.open("src/pages/error/404.html");
		if (file.is_open() == false)
            throw std::runtime_error("Error: file not found error2222");
        _buffer << file.rdbuf();
        file.close();
    }
}