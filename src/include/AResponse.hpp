#pragma once

#include <iostream> 
#include <string> 
#include <ctime>
#include <sstream>
#include <fstream>

#define TIME_SIZE 40
/*
1. **Status-Line (상태 라인):** HTTP 응답의 첫 번째 줄로, 프로토콜 버전, 상태 코드 및 상태 메시지로 구성됩니다. <버전> <상태 코드> <사유 구절>
2. **Date (날짜):** 응답이 생성된 시간을 나타내는 정보입니다.
3. **Server (서버):** 웹 서버의 소프트웨어 또는 버전과 같이 서버에 대한 정보를 포함할 수 있습니다.
4. **Content-Type (컨텐트 타입):** 응답 바디의 데이터 타입을 지정합니다.
5. **Content-Length (컨텐트 길이):** 응답 바디의 길이를 바이트 단위로 나타냅니다.
*/
class Request;

class AResponse
{
	protected:
		std::string		_version;
		std::string		_stateCode;
		std::string		_reasonPhrase;

		std::time_t		_date;

		std::string 	_serverName;

		std::string		_contentType;
			
		unsigned int	_contentLength;

		std::stringstream	_buffer;

		// 삭제한 request 를 가리킬 가능성이 있는가??
		Request*		_request;	
		
		AResponse(const AResponse& src); 
		AResponse& operator=(AResponse const& rhs);
	public:
		AResponse();
		virtual ~AResponse();
		AResponse(Request* request);
		std::string getDate() const;
		virtual void createResponseHeader() =0;
		virtual void createResponseMain();
		void stamp() const;
		const std::stringstream& getBuffer() const;
};

// response data 를 만들고 stamp 를 찍는다.

// // 형식
// <버전> <상태 코드> <사유 구절>
// <헤더>

// <본문>

// // ex
// HTTP/1.1 200 OK
// Content-type: text/html
// Context-length: 42
// ...

// Body line1
// Body line2
// ...

// ### 응답 메시지 필수 항목
// 1. **Status-Line (상태 라인):** HTTP 응답의 첫 번째 줄로, 프로토콜 버전, 상태 코드 및 상태 메시지로 구성됩니다. 예를 들면 다음과 같습니다.
	
//     HTTP/1.1 200 OK
	
// 2. **Date (날짜):** 응답이 생성된 시간을 나타내는 정보입니다.
	
//     Date: Tue, 20 Jul 2023 12:34:56 GMT
	
// 3. **Server (서버):** 웹 서버의 소프트웨어 또는 버전과 같이 서버에 대한 정보를 포함할 수 있습니다.
	
//     Server: Apache/2.4.38 (Unix)
	
// 4. **Content-Type (컨텐트 타입):** 응답 바디의 데이터 타입을 지정합니다.
	
//     Content-Type: text/html; charset=UTF-8
	
// 5. **Content-Length (컨텐트 길이):** 응답 바디의 길이를 바이트 단위로 나타냅니다.
	
//     Content-Length: 1024 

// 이러한 헤더들은 대부분의 HTTP 응답에서 기본적으로 포함되어야 하는 필수 정보입니다. 하지만 일부 헤더들은 특정 상황에서 선택적으로 사용될 수 있습니다. 특히 캐싱과 관련된 헤더들(Cache-Control, Expires, Last-Modified 등)은 캐시 정책에 따라 필요 여부가 달라질 수 있습니다. 따라서 특정 상황에 맞춰서 적절한 헤더들을 포함하여 응답 메시지를 구성해야 합니다.