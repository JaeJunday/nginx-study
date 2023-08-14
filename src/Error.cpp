#include "Client.hpp"

// Error::Error(Request* request, int kq) : AResponse(kq)
// {
// 	_request = request;
// }
void Client::pushErrorBuffer(std::string body, int errnum)
{
	std::stringstream responseData;

	responseData << "HTTP/1.1" << " " << errnum << " " << _reasonPhrase << "\r\n";
	responseData << "Content-Type: text/html; charset=UTF-8" << "\r\n";
	responseData << "Server: My Server" << "\r\n";
	responseData << "Referrer-Policy: no-referrer" << "\r\n";
	responseData << "Content-Length: " << _contentLength << "\r\n";
	responseData << "Date: " << Client::getDate() << "\r\n\r\n";
	responseData << body;
	_buffer << responseData.str();
}

void Client::makeErrorPage(int errnum)
{
	std::ifstream       file;
	std::string         filePath;
	std::stringstream   body;

	// 400 403 404 405 413
	switch (errnum) {
		case 400:
			_reasonPhrase = "Bad Request"; break;
		case 401:
			_reasonPhrase = "Unauthorized"; break;
		case 403:
			_reasonPhrase = "Forbidden"; break;
		case 404:
			_reasonPhrase = "Not Found"; break;
		case 405:
			_reasonPhrase = "Method Not Allowed"; break;
		case 408:
			_reasonPhrase = "Request Timeout"; break;
		case 411:
			_reasonPhrase = "Length Required"; break;
		case 413:
			_reasonPhrase = "Content Too Large"; break;
		case 505:
			_reasonPhrase = "HTTP Version Not Supported"; break;
	}
	filePath = "./src/pages/error/404.html";
	file.open(filePath.c_str());
	if (file.is_open())
	{
		body << file.rdbuf();
		_contentLength += body.str().length();
		file.close();
		pushErrorBuffer(body.str(), errnum);
	}
}