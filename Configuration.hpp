#pragma once

#include "Server.hpp"
#include <iostream>
#include <string>
#include <stack>
#include <fstream>

class Configuration
{
private:
	std::stack<std::string> _parenticts;
	bool                    _locationFlag;
	bool                    _serverFlag;
	unsigned int            _count;
public:
	Configuration(); // Default Constructor
	~Configuration(); // Destructor
	Configuration(const Configuration& other); // Copy Constructor
	Configuration& operator=(const Configuration& other); // Assignment Operator

	void parsing(const std::string& filePath);
	void push(const std::string& input);
	void pop();
};

// exception 
// 서버, 로케이션 스트링 넣음
// 열린괄호 넣으면 열린괄호 넣음
// 닫힌괄호 받으면 열린괄호 뺌

// 플래그
// 서버, 로케이션, 카운트
// 의사코드
// 서버를 넣을떄 : 서버를 만나면 서버플래그랑 로케이션플래그 검사한 후 서버 스트링을 스택에 넣고 카운트를 올린다. 플래그를 서버로 바꾼다
// 로케이션을 넣을때 : 로케이션을 만나면 서버플래그랑 로케이션플래그를 검사한 후 로케이션 스트링을 스택에 넣고 카운트를 올린다 플래그를 로케이션으로 바꾼다
// 열린괄호를 만나면 열린괄호를 스택에 넣는다.
// 닫힌괄호를 만나면 열린괄호 - 스트링 순서를 맞추어서 스택에서 뺀다. 카운트를 내린다. 플래그를 끈다. 
// 다 읽었을때 카운트가 남아 있으면 에러
// 카운트가 0일때 스택이 비어있지 않으면 에러

// ;룰 끝 
//find로 ;을 찾는다. 못찾으면 에러
//;뒤에 널문자랑 닫는 괄호문자가 아니면 에러

//서버 안에 서버  서버가 꺼져있는지
//로케이션 안에 서버 로케이션이 켜져있는지 

//로케이션 안에 로케이션 로케이션이 꺼져있는지 
//서버 밖에 로케이션 서버가 꺼져있는지 

// server 조건식인 경우
// { 시작전에 문자열이 들어온다 -> error
