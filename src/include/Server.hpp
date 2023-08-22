#pragma once

#include "enum.hpp"
#include <iostream>
#include <locale>
#include <string>
#include <map>
#include <vector>

class Operation;

/*
	[feat] - kyeonkim
	- conf server에 root 키워드를 추가해야한다.
	- root 에 따라 서로 다른 파일을 호출해서 보여줘야한다.
	- 예를 들어, conf에서 server root 를 pages로 설정할 경우 
	  루트로 요청을 보낸다면 pages에 있는 hello.html을 보여주는데
	  conf에서 server root 를 pages2로 설정할 경우에 루트로 요청을 보내면 
	  pages2 폴더 안에 있는 파일을 보여줘야한다.
	- 추가해도 되고 안해도 되는 부분. 근데 코드 상으로는 추가할려고 했음.
*/

struct Location
{
	std::string					_path;
	std::string					_root;
	std::string					_index;
	std::string					_autoindex;
	std::string					_upload;
	std::string					_py;
	std::string 				_cgiParam;
	std::string 				_bla;
	std::string					_php;
	std::string					_clientMaxBodySize; // 본문 사이즈 제한
	std::vector<std::string>	_limitExcept; // 메서드 제한
	std::string					_tryFiles;
};

class Server {
	private:
		std::vector<std::string>	_serverName;
		std::vector<std::string>  	_errorPage;
		std::string					_listen;
		std::string					_root;
		std::string					_index;
		std::string					_clientMaxBodySize;
		std::vector<Location>		_location;
    	int 						_socket;
	public:
		Server();
		Server(const Server& other);
		Server& operator=(const Server& other);
		~Server();
		void setValue(int index, std::string& value);
		std::string getValue(int index) const;
		void setLocation(const Location& location);
		const Location& getLocation(int index) const;
		int getLocationSize() const;
		void setSocket(int fd);
		int getSocket() const;
		const std::string& getRoot() const;
		const std::string& getIndex() const;
		const std::vector<Location>& getLocations() const;
		// const std::string& getCgiParam() const;
};
