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
    // std::ifstream	file;
    // std::stringstream tmp;

    // 경로에 따라서 맞는 경로의 파일을 오픈
    std::string path = findLocationPath();
	// --------------------------------------------------- testcode
	// std::cerr << "찾은경로 : " << path << std::endl;
	// --------------------------------------------------- 
	openPath(path);
	_buffer << "Content-Length: " << _contentLength << "\r\n\r\n";
}

void Get::openPath(const std::string& path) const
{
    std::ifstream	file;
    std::stringstream tmp;
	DIR *dir_stream = opendir(path.c_str());
	if (dir_stream == NULL) 
	{
		close(dir_stream);
		file.open(dirname);
		if (file.is_open() == true)
		{
			tmp << file.rdbuf();
			_contentLength += tmp.str().length();
			_buffer << tmp;
			file.close();
		}
		else 
			//server errorcode
	}
	else // 만약에 오토 인덱스라면 다 읽어서 보여주기 그게 아니면 보여줄거없음
	{
		if (_request->getLocation()._autoindex == "on")
		{
			struct dirent *entry;
			while (entry != NULL)
			{
				*entry = readdir(dir_stream);
				_buffer << std::string(entry.d_name) + "\n";
			}
			_contentLength = _buffer.str().length();
			closedir(dir_stream);
		}
		else
		{
			// 보여줄게 없다는 메시지를 띄운다(state code는 200으로 처리한다. 왜냐하면 폴더는 있기 때문에).	
		}
	}
}

std::string Get::findLocationPath() const
{
	const Server server = _request->getServer();
	const std::vector<Location>& locations = server.getLocations();
	std::string result = _request->getRequestUrl();
	Location location;
	int length = 0;
	if (result.empty())
	{
		// 경로가 없는 경우 errorcode
	}
	for (int i = 0; i < locations.size(); ++i) {		
		int pathLength = locations[i]._path.length();
		if (_request->getRequestUrl().compare(0, pathLength, locations[i]._path) == 0)
		{
			if (length < pathLength)
			{
				length = pathLength;
				location = locations[i];
			}
		}
 	}
	if (length == false)
	{
		// std::cerr << "no location" << std::endl;
		// no location errorcode
	}
	_request->setLocation(location);
	if (!location._root.empty())
	{
		result.erase(0, length);
		result = location._root + result;
	}
	else if (!server.getRoot().empty())	
	{
		result.erase(0, length);
		result = server.getRoot() + result;
	}
	if (!location._index.empty())
		result += "/" + location._index;
	else if (!server.getIndex().empty())
		result += "/" + server.getIndex();
	return result;

//------------------------------------------------------------------- testcode
    // std::cerr <<  "ㅋㅋ 반대임 요청은 /로 오고 컨피그는 \\로 루트 저장해서 못찾고있는거였음" << std::endl; // ㅋㅋㅋ - kyeonkim
	// std::cerr << "requesturl: " << _request->getRequestUrl() << std::endl;
	// std::cerr << "locationPath: " << location._path << std::endl;
	// std::cerr << std::endl;
	
	// std::cerr << "socket: " << server.getSocket() << std::endl;
	// std::cerr << "요쳥 url 문자열: " << _request->getRequestUrl() << std::endl;
	// std::cerr << "로케이션 경로 문자열: " << location._path << std::endl;
	// std::cerr << std::endl;

	// std::cerr << "로케이션 루트 문자열: " << location._root << std::endl;
	// std::cerr << "서버 루트 문자열: " << server.getRoot() << std::endl;
	// std::cerr << std::endl;

	// std::cerr << "로케이션 인덱스 문자열: " << location._index << std::endl;
	// std::cerr << "서버 인덱스 문자열: " << server.getIndex() << std::endl;
	// std::cerr << std::endl;
	// std::cerr << "바뀔 문자열" << std::endl;
//------------------------------------------------------------------- 
}

void Get::createResponseMain()
{}
