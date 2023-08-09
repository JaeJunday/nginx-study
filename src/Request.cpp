#include "Request.hpp"

Request::Request(int socket, const Server& server)
    : _server(server), _state(0), _socket(socket), _port(0), _contentLength(0), _eventState(0), _bodyIndex(0)
{
}

void Request::checkMultipleSpaces(const std::string& str)
{
    char prev = '\0';
    for (size_t i = 0; i < str.size(); ++i) {
        if ((prev == ' ' || prev == '\n') && prev == str[i])
            throw std::runtime_error("Error: Invalid Octet Space or newLine");
        prev = str[i];
    }
}

void Request::setRequestLine(std::string& requestLine)
{
    checkMultipleSpaces(requestLine);
    std::vector<std::string> token = util::getToken(requestLine, " ");

    // testcode 
    int         bodySize;
    // std::cerr << "========================requestLine========================" << std::endl;
    // std::cerr << requestLine << std::endl;

    if (token.size() != 3)
    {
        std::cerr << "Error: Request Line size error" << std::endl;
        throw 400;
    }
    if (!(token[0] == "GET" || token[0] == "DELETE" || token[0] == "POST" || token[0] == "PUT"))
        throw 405;
    if (token[2] != "HTTP/1.1")
        throw 400;
    _method = token[0];
    _requestUrl = token[1];
    _version = token[2];
}

void Request::setFieldLine(std::string& fieldLine)
{
    checkMultipleSpaces(fieldLine);
    std::vector<std::string> token = util::getToken(fieldLine, ": ");
    token[1].erase(0, 1);
    if (token.size() != 2)
        throw 401;
        //throw std::runtime_error("Error: Header error");
    if (token[0].empty() || token[1].empty())
        throw 402;
        // throw std::runtime_error("Error: Header Key Value Empty");
    if (token[0].find(' ') != std::string::npos)
        throw 403;
        // throw std::runtime_error("Error: Header Key have space");
    if (token[0] == "Host") {
        size_t mid = token[1].find(":");
        if (mid == std::string::npos)
        {
            std:: cerr << "ERROR : HOST ERROR" << std::endl;
            throw 404;
        }
        _ip = std::string(token[1], 0, mid);
        _port = util::stoui(std::string(token[1], mid + 1, token[1].size() - (mid + 1)));
    }
    if (token[0] == "Content-Type")
    {
        const std::string hash = "boundary=";
        if (int index = token[1].find(hash) != std::string::npos)
            _boundary = "--" + std::string(token[1], index + hash.length(), token[1].size() - index);
        else 
            _contentType = token[1]; 
    } 
    if (token[0] == "Content-Length")
        _contentLength = util::stoui(token[1]);
    if (token[0] == "Transfer-Encoding")
        _transferEncoding = token[1];
    if (token[0] == "Connection")
        _connection = token[1];
}


// 헤더 + 본문 길이 = buffersize
// 본문길이  = contentLength
// 본문 스타트 길이 = bodyindex

// buffersize - bodyindx == contesntLength : ok
void Request::headerParsing(char* buf, intptr_t size)
{
    // testcode
    // std::cerr << "========================parsing========================" << std::endl;
    // 헤더 끝줄 찾기
    std::string buffer(buf, size);
    _headerBuffer += buffer;
    int headerBoundary = _headerBuffer.find("\r\n\r\n");
    if (headerBoundary == std::string::npos)
        return ;
    std::cout << RED << "testcode " << "====_headerBuffer\n" << _headerBuffer << RESET << std::endl;
    _state = request::DONE;
    // 첫번째 라인일때 - Refactoring 나중에 함수로 뺀다 - kyeonkim
    // 한줄 밖에 없을 때 find 조건이 -1이 나와버림. 한줄도 넣어서 검사해야함 - kyeonkim
    int endLine = _headerBuffer.find("\r\n");
    std::string requestLine(_headerBuffer, 0, endLine);
    setRequestLine(requestLine);

    // 둘째줄부터 끝줄까지
    //int secondLine = buffer.find("\r\n", endLine + 2);
    int newEndLine;
    endLine += 2;
    while (endLine < headerBoundary) { 
        newEndLine = _headerBuffer.find("\r\n", endLine);
        std::string fieldLine(_headerBuffer, endLine, newEndLine - endLine);

        setFieldLine(fieldLine);
        endLine = newEndLine + 2;
    }
    // int start = headerBoundary + 4;
    _bodyIndex = headerBoundary + 4;
    // if (_headerBuffer.size() - start != 0)
    //     _buffer = _headerBuffer.substr(start, _headerBuffer.size() - start);
    // if (_method == "POST")
    //     _state = request::POST;
}

std::string removeSpecificCharacter(std::string str, char ch)
{
	size_t pos = str.find(ch);

	while (pos != std::string::npos) {
		str.erase(pos, 1);
		pos = str.find(ch, pos);
	}
	return (str);
}

/*
1. 리퀘스트 메시지 처음부터 읽어오면서 첫 번째 rnrn 찾기 (여기까지가 헤더!)
2. 헤더 파싱해서 어떤 메시지인지 확인(청크드인지 콘텐츠 렝스가 있는지 확인)

3. (청크드면 1에 나온 rnrn이후에 바로 바디가 오는거니까) 들어온 문자열을 hex->int로 바꿔서 숫자를 보고, 그 뒤에 나오는 문자가 rn인지 확인
    - 여기에서 rn으로 끝나지 않는 경우에 메시지를 덜 받아왔을 가능성이 있으니(숫자 받아오다가 잘렸거나 r까지만 들어왔다거나 등등) recv 더 해줄 필요가 있을 수 있어요



    
4. rn이 나오는 경우 int로 바꾼 숫자+2만큼의 버퍼기 채워졌을 때(recv로 받아왔을 때) 확인해서 진행
    - 0보다 큰 경우: 해당 숫자만큼 (내용을 신경쓰지 않고) 모두 바디로 처리, 바디처리한 내용 직후에 rn이 나오는지 체크해서 없을경우 에러처리, rn이 있으면 rn이후에 다시 3번으로 돌아가 반복
    - 0인 경우: 그 다음 2자를 더 받아와서 총 0rnrn으로 끝나는지 확인-> 리퀘스츠 수신 끝
    - 이 경우에 각 청크드메시지가 rn혹은 rnrn으로 끝나는 걸 감안해서 int로 변환한 수 +2만큼의 바퍼를 받아와 먼저 rn까지 확인하고, 0인 경우에는 변환한 수(0) +4만큼의 버퍼를 받아와서 rnrn으로 끝나는지 검사

청크드 바디가 들어오는 시점부터 4번의 옵션을 모두 고려해주시면 괜찮을거같아요
숫자 먼저 체크하고, 숫자에 따라
- 1이상: 바디채우고 rn으로 끝나는지 체크하고 다시 숫자 체크로 돌아감
- 0: rnrn으로 끝나는지 체크하는 과정을 0블럭이 들어올때까지 반복하는 로직으로 진행
*/

// _buffer, _chunkedIndex, body, bodySize
bool Request::parseChunkedData()
{
    size_t  bodySize;
    size_t  index;
    char*   endptr;

    if (_chunkedIndex == false)
        _chunkedIndex = _bodyIndex;
    index = _buffer.find("\r\n", _chunkedIndex);
    if (index != std::string::npos)
    {
        bodySize = std::strtol(_buffer.c_str() + _chunkedIndex, &endptr, HEX);
        size_t bodyStart = index + 2; 
        if (bodySize == 0 && _buffer.find("\r\n", bodyStart) != std::string::npos)
            return true;
        if (bodyStart + bodySize <= _buffer.length())
        {
            std::string perfectBody = _buffer.substr(bodyStart, bodySize);
            // _response->cgi(perfectBody); //cgi
            _chunkedIndex = bodyStart + bodySize;
        }
        else
            return false;
    }
}

// bool Request::checkDeque(Request* req, int& lenToSave, std::string& updatedBuffer)
// {
// 	bool head = false;
// 	if (_chunkedBuffer.empty() == false && _chunkedBuffer.back()._len != _chunkedBuffer.back()._saved.length())
// 	{
// 		Buffer prevBuffer = _chunkedBuffer.back();
// 		_chunkedBuffer.pop_back();
// 	    lenToSave = prevBuffer._len;
// 		if (lenToSave != -1)
// 			head = true;
// 		updatedBuffer = prevBuffer._saved;
// 	}
// 	return head;
// }

// void Request::endChunkedParsing(Request* req)
// {
// 	std::deque<struct Buffer> chunked = _chunkedBuffer;
// 	int totalLen = -1;
// 	std::string mergedBuffer;
//     std::vector<std::string> url = util::getToken(_requestUrl, "/");

// 	// \r\n\r\n 만 남았는지 check하는 로직 추가하고, 
// 	// 그 외의 데이터가 들어 왔으면 다시 버퍼에 붙여서 parsing에 들어 가게 한다.


//     if (url.size() >= 1)
//         _chunkedFilename = url[url.size() - 1];
// 	while (chunked.size())
// 	{
// 		struct Buffer buf = chunked.front();
// 		totalLen += buf._len;
// 		mergedBuffer += buf._saved;
// 		chunked.pop_front();
// 	}
//     //if (chunked.size() == 0)
//     //    totalLen = 0;

	
// 	// -totalLen 이 0일 때 오류 처리 해야함 - semikim
	
// 	// req->changeBuffer(mergedBuffer);
// 	// req->setContentLength(totalLen);
//     _buffer = mergedBuffer;
//     _contentLength = totalLen;
//     // ---------------------------------------- testcode
//     std::cout << RED << "testcode " << "totalLen:" << totalLen << RESET << std::endl;
//     std::cout << RED << "testcode " << "length:" <<  _contentLength << RESET << std::endl;
//     //std::cerr << "mergedBuffer\n" << mergedBuffer << std::endl;
//     // std::cerr << "=============chunked data====================" << std::endl;
//     // std::cerr << _chunkedFilename << std::endl;
//     // std::cerr << _contentType << std::endl;
//     // std::cerr << _buffer << std::endl;
// }

// bool Request::parseChunkedData(Request* req, const std::string& updatedBuffer)
// {
//     std::string str;
//     int start = 0, i = 0, e = 0;
//     bool head = false;
//     int lenToSave = -1;

//     std::string tmp;
//     head = checkDeque(req, lenToSave, tmp);
//     std::string buffer = tmp + updatedBuffer;
//     std::cerr << RED << "testcode " << "buffer : "<< buffer << RESET << std::endl;
//     while (true)
//     {
//         if (head == false)
//         {
// 			i = buffer.find("\r\n", start);
//             if (i != std::string::npos)
//             {
//                 char *end = NULL;
//                 str = buffer.substr(start, i - start);
//                 if (str.empty() == false)
//                 {
//                     lenToSave = std::strtol(str.c_str(), &end, 16);
//                     std::cerr << GREEN << "testcode" << " => lenToSAve:"  << lenToSave << RESET << std::endl;
//                 	if (lenToSave == 0)
//                 	{
// 						endChunkedParsing(req);
//                         // updatedBuffer 에 데이터가 남아 있다면 다음에 들어오는 요청 헤더일수도 있기 때문에 보관해야 한다-semikim
//                         // \r\n\r\n 을 제거하고 어떻게 이어주지
//                     	return (true);
//                 	}
// 				}
//                 head = true;
//                 i += 2;
//             }
//             else
//             {
//                 Buffer buf;
//                 str = buffer.substr(start, std::string::npos);
//                 if (str.empty() == false)
// 				{
// 					buf._saved = str;
//                 	buf._len = 0;
//                 	_chunkedBuffer.push_back(buf);
// 				}
//                 break;
//             }
//         }
//         if (head == true)
//         {
// 			Buffer buf;
// 			buf._len = lenToSave;
//             size_t st = i;
//             while (1)
//             {
//                 e = buffer.find("\r\n", i);
//                 if (e == std::string::npos)
//                     break;
//                 if (e - st == lenToSave)
//                     break;
//                 else
//                     i = e + 2;
//             }
// 
//             if (e != std::string::npos)
//             {
//                 str = buffer.substr(st, e - st); 
//                 if (str.empty() == false)
//                 {
//                     if (str.size() == lenToSave)
//                     {
//                         buf._saved = str;
//                 		_chunkedBuffer.push_back(buf);
//                     }
//                     else //testcode
//                         std::cerr << "len false" << std::endl;
//                 }
//                 head = false;
//                 if (e != std::string::npos)
//                     e += 2;
//                 else
//                     break;
//             }
//             else
//             {
//                 str = buffer.substr(st, std::string::npos); 
//                 if (str.empty() == false)
//                 {
//                     buf._saved = str;
//                     _chunkedBuffer.push_back(buf);
//                 }
//                 break;
//             }
//         }
//         start = e;
//     }
//     return (false);
// }

// std::deque<struct Buffer>& Request::getChunkedBuffer()
// {
// 	return _chunkedBuffer;
// }

void Request::makeResponse(int kq)
{
	if (_method.empty())
		throw 405;
	if (_method == "GET")
		_response = new Get(this, kq);
	else if (_transferEncoding == "chunked")
		_response = new Chunked(this, kq);
	else if (_method == "POST" || _method == "PUT")
		_response = new Post(this, kq);
	else if (_method == "DELETE")
		_response = new Delete(this, kq);
    _state = request::DONE;
}

void Request::clearRequest()
{
    _location = NULL;
    _state = 0;
    _headerBuffer = "";
    _buffer = "";
	_method = "";
	_requestUrl = "";
	_version = "";
	_connection = "";
	_contentType = "";
	_contentLength = 0;
	_transferEncoding = "";
	_boundary = "";
	_chunkedFilename = "";
    _bodyIndex = 0;
}

void Request::setBuffer(char *buffer, int size)
{
	_buffer.append(buffer, size);
}

void Request::setEventState(int eventState)
{
    _eventState = eventState;
}

const std::string& Request::getConnection() const
{
    return _connection;
}

const std::string& Request::getTransferEncoding() const
{
    return _transferEncoding;
}

unsigned int Request::getContentLength() const
{
    return _contentLength;
}

const std::string& Request::getIp() const
{
    return _ip;
}

const std::string& Request::getMethod() const
{
    return _method;
}

const std::string& Request::getVersion() const
{
	return _version;
}

const std::string& Request::getRequestUrl() const
{
    return _requestUrl;
}

int Request::getState() const
{
    return _state;
}


int Request::getSocket() const
{
    return _socket;
}

const std::string& Request::getBuffer() const
{
    return _buffer;
}

const Server& Request::getServer() const
{
    return _server;
}


Location* Request::getLocation() const
{
    return _location;
}
     
void Request::setLocation(Location* location)
{
    _location = location;
}

const std::string& Request::getBoundary() const
{
	return _boundary;
}

int Request::getEventState() const
{
    return _eventState;
}

const std::string& Request::getContentType()
{
    return _contentType;
}


const std::string& Request::getChunkedFilename()
{
    return _chunkedFilename;
}

int Request::getBodyIndex() const
{
    return _bodyIndex;
}

AResponse* Request::getResponse() const
{
    return _response;
}
