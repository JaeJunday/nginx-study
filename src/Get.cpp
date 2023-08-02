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

/*

	- loop을 돌면서 location _path 의 길이를 세서 location _path 길이까지만 request _url과 비교를 한다.
	- if request _url와 location _path이 같은데 길이가 이전 길이보다 길다면 해당 _path의 location 구조체를 가지고 있는다.
	- location을 찾았을때 || 가지고 있을 때, 
	- location root가 있을 경우,
		request url 에서 location _path와 겹치는 부분을 root value 로 치환하여
	- location root가 없을 경우,
		
	- 파일일 경우,
		해당 파일이 있는지 찾는다.
	- 폴더일 경우,
		index 가 있다면, index 붙여서 해당 파일이 있는지 찾는다.
		index 가 없을 경우, 보여줄 게 없다는 메시지를 띄운다(state code는 200으로 처리한다. 왜냐하면 폴더는 있기 때문에).
	
request 요청이 dir 인 경우,
---------------------------
	| path | root | index |
       o      o       o         : 동일한 부분을 root로 치환한 url + index 경로에 해당 파일이 있는지 찾는다.
       o      o       x         : 동일한 부분을 root로 치환하고 autoindex 가 있으면 리스트로 보여주고 없으면 보여줄 게 없다는 메시지를 띄운다(state code는 200으로 처리한다. 왜냐하면 폴더는 있기 때문에).
	   o      x       o         : path + index 경로에 해당 파일이 있는지
	   o      x       x         : autoindex 가 있으면 리스트로 보여주고 없으면 보여줄 게 없다는 메시지를 띄운다(state code는 200으로 처리한다. 왜냐하면 폴더는 있기 때문에).
	   x      x       x         : return 404
--------------------------
auto index off && index X = 보여줄게 없다?
auto index on && index X = 목록 보여주기

index 옵션이 있고 request 요청이 dir 인 경우, 경로 끝에 index value 값을 무조건 붙여서 찾는다. - kyeonkim

ㅣ
*/


void Get::createResponseHeader()
{
	_buffer << _version << " " << _stateCode << " " << _reasonPhrase << "\r\n";
	_buffer << "Date: " << getDate() << "\r\n";
	_buffer << "Server: " << _serverName << "\r\n";
	_buffer << "Content-Type: " << _contentType << "\r\n";
    std::ifstream	file;
    std::stringstream tmp;

    // 경로에 따라서 맞는 경로의 파일을 오픈
    std::string found = findLocationPath();
	std::cerr << "찾은경로 : " << found << std::endl;
	found = "find"; // 임시 데이터 - kyeonkim

    if (found.length() > 0 || _request->getRequestUrl() == "/")
	{
		Location location;
		const Server server = _request->getServer();
		std::string filename;
		for (int i = 0; i < server.getLocationSize(); ++i) 
		{
			location = server.getLocation(i);
			// .                    -location _root
			filename = findFilename("./src/pages" + location._path);
			filename = "./src/pages/hello.html"; // 임시 데이터 - kyoenkim
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

std::string Get::findLocationPath() const
{
	const Server server = _request->getServer();
	const std::vector<Location>& locations = server.getLocations();
	Location location;
	int min = 0;
		// index
		// root 	
  
	for (int i = 0; i < locations.size(); ++i) {		
		int pathLength = locations[i]._path.length();
		if (_request->getRequestUrl().compare(0, pathLength, locations[i]._path) == 0)
		{
			if (min < pathLength)
			{
				min = pathLength;
				location = locations[i];
			}
		}
	}	

	if (min == false)
	{
		std::cerr << "no location" << std::endl;
		// error code bad request? 맞는 로케이션이 없는 경우
	}
	std::string result;

//------------------------------------------------------------------- testcode
    std::cerr <<  "ㅋㅋ 반대임 요청은 /로 오고 컨피그는 \\로 루트 저장해서 못찾고있는거였음" << std::endl;
	location = locations[0];
	std::cerr << "requesturl: " << _request->getRequestUrl() << std::endl;
	std::cerr << "locationPath: " << locations[0]._path << std::endl;
	std::cerr << std::endl;
	
	std::cerr << "socket: " << server.getSocket() << std::endl;
	std::cerr << "요쳥 url 문자열: " << _request->getRequestUrl() << std::endl;
	std::cerr << "로케이션 경로 문자열: " << location._path << std::endl;
	std::cerr << std::endl;

	std::cerr << "로케이션 루트 문자열: " << location._root << std::endl;
	std::cerr << "서버 루트 문자열: " << server.getRoot() << std::endl;
	std::cerr << std::endl;

	std::cerr << "로케이션 인덱스 문자열: " << location._index << std::endl;
	std::cerr << "서버 인덱스 문자열: " << server.getIndex() << std::endl;
	std::cerr << std::endl;

	// std::cerr << "바뀔 문자열" << std::endl;
//------------------------------------------------------------------- 

	// min이 false 가 아니라면 가장 fit 한 로케이션을 찾은 상태
	if (location._root.empty() != true)
	{
	}
	else if (server.getRoot().empty() != true)	
	{
	}

	if (location._index.empty() != true)
	{
	}
	else if (server.getIndex().empty() != true)
	{
	}
	return result;
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
