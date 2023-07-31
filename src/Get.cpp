#include "Get.hpp"
#include "Request.hpp"
#include "Server.hpp"

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
Content-type: text/html
Context-length: 42
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
void Get::createResponseHeader(const Server& server)
{
	_buffer << _version << " " << _stateCode << " " << _reasonPhrase << "\r\n";
	_buffer << "Date: " << getDate() << "\r\n";
	_buffer << "Server: " << _serverName << "\r\n";
	_buffer << "Content-Type: " << _contentType << "\r\n";
    std::ifstream	file;
    std::stringstream tmp;
    // 경로에 따라서 맞는 경로의 파일을 오픈
    std::string found = findLocationPath(server);
    if (found.empty() != true)
	{
		std::string filename;
		for (int i = 0; i < _request->getFilesSize(); ++i) {
            std::vector<PostData> files = _request->getFiles();
			filename = findFilename(files);
			file.open("src/pages/hello.html");
			if (file.is_open() == false)
				continue;
				// throw std::runtime_error("Error: file not found error");
			tmp << file.rdbuf();
			_contentLength += tmp.str().length();
			file.close();	
		}
	}
    else if (_request->getRequestUrl() == "/")
	{

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

const std::string& findFilename(std::vector<PostData>& files)
{
    for (int i = 0; i < files.size(); ++i)
    {
        
			// opendir 디렉터리 파일 이름 읽는 예제
		const char *dirname = "."; // 현재 디렉토리를 열도록 지정
		DIR *dir_stream = opendir(dirname);
		if (dir_stream == NULL) {
			return ;
		}
		struct dirent *entry;
		while ((entry = readdir(dir_stream)) != NULL) {
			printf("%s\n", entry->d_name);
		}
		closedir(dir_stream);
		return 0;
    }
}

const std::string& Get::findLocationPath(const Server& server) const
{
    std::vector<std::string> path = util::getToken(const_cast<std::string&>(_request->getRequestUrl()), "\\");
    std::string tmp;
    for (int i = path.size() - 1; i > -1; --i) {
        if (i != path.size() - 1)
            tmp += "\\";
        tmp += path[i];   
	    Location location;
		for (int i = 0; i < server.getLocationSize(); ++i)
		{
			location = server.getLocation(i);
			if (location._path == tmp)
			return true;
		}
    }
	return false;
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
		file.open("src/pages/error/404.html");
		if (file.is_open() == false)
            throw std::runtime_error("Error: file not found error2222");
        _buffer << file.rdbuf();
        file.close();
    }
}