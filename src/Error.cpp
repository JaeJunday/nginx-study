#include "Error.hpp"

void pushErrorBuffer(ErrorData& data, std::string body)
{
    std::stringstream responseData;

    responseData << "HTTP/1.1" << " " << data._stateCode << " " << data._reasonPhrase << "\r\n";
	responseData << "Content-Type: text/html; charset=UTF-8" << "\r\n";
	// responseData << "Server: My Server" << "\r\n";
    responseData << "Referrer-Policy: no-referrer" << "\r\n";
	responseData << "Content-Length: " << data._contentLength << "\r\n";
	responseData << "Date: " << AResponse::getDate() << "\r\n\r\n";
	responseData << body;
    data._buffer = responseData.str();
    std::cerr << data._buffer << std::endl;
}

/*
HTTP/1.1 200 OK
Date: Mon, 07 Aug 2023 17:15:02 GMT
Server: SpiderMen/1.5.2
Connection: keep-alive
Content-Length: 19
Content-Type: text/plain
No data received.
*/

void sendErrorPage(int fd, int errnum)
{
    ErrorData           errorData;
    std::ifstream       file;
    std::string         filePath;
    std::stringstream   body;

    memset(&errorData, 0, sizeof(errorData));
    // 400 403 404 405 413
    switch (errnum) {
        case 400:
            errorData._reasonPhrase = "Bad Request"; break;
        case 401:
            errorData._reasonPhrase = "Unauthorized"; break;
        case 403:
            errorData._reasonPhrase = "Forbidden"; break;
        case 404:
            errorData._reasonPhrase = "Not Found"; break;
        case 405:
            errorData._reasonPhrase = "Method Not Allowed"; break;
        case 408:
            errorData._reasonPhrase = "Request Timeout"; break;
        case 411:
            errorData._reasonPhrase = "Length Required"; break;
        case 413:
            errorData._reasonPhrase = "Content Too Large"; break;
        case 505:
            errorData._reasonPhrase = "HTTP Version Not Supported"; break;
    }
    errorData._stateCode = errnum;
    filePath = "./src/pages/error/404.html";
    file.open(filePath.c_str());
    if (file.is_open())
    {
        body << file.rdbuf();
        errorData._contentLength += body.str().length();
        file.close();
        pushErrorBuffer(errorData, body.str());
        send(fd, errorData._buffer.c_str(), errorData._buffer.length(), 0);
    }
}

/*
HTTP/1.0 400 Bad Request
Content-Type: text/html; charset=UTF-8
Referrer-Policy: no-referrer
Content-Length: 1555
Date: Mon, 07 Aug 2023 05:40:36 GMT
*/
/*
HTTP/1.1 400 Bad Request
Content-Type: text/html; charset=UTF-8
Referrer-Policy: no-referrer
Content-Length: 346
Date: Mon, 07 Aug 2023 14:44:52 GMT
*/