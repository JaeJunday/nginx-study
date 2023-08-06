#include "Error.hpp"

void pushErrorBuffer(ErrorData& data, std::string body)
{
    std::stringstream responseData;

    responseData << "HTTP/1.1" << " " << data._stateCode << " " << "Not Found" << "\r\n";
	// responseData << "Date: " << getDate() << "\r\n";
	responseData << "Server: My Server" << "\r\n";
	responseData << "Content-Type: text/html" << "\r\n";
	responseData << "Content-Length: " << data._contentLength << "\r\n\r\n";
	responseData << body;
    data._buffer = responseData.str();
    std::cerr << data._buffer << std::endl;
}

void sendErrorPage(int fd, int errnum)
{
    ErrorData           errorData;
    std::ifstream       file;
    std::string         filePath;
    std::stringstream   body;

    memset(&errorData, 0, sizeof(errorData));
    switch (errnum) {
        case 400:
        case 404:
        case 405:
        {
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
            break;
        }
    }
}