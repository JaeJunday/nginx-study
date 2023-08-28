#include "Client.hpp"

void Client::pushErrorBuffer(std::string body, int _stateCode)
{
	std::stringstream responseData;

	responseData << "HTTP/1.1" << " " << _stateCode << " " << _reasonPhrase << "\r\n";
	responseData << "Content-Type: text/html; charset=UTF-8" << "\r\n";
	responseData << "Server: My Server" << "\r\n";
	responseData << "Referrer-Policy: no-referrer" << "\r\n";
	if (!(_stateCode == 400 || _stateCode == 404 || _stateCode == 500 || _stateCode == 413))
		_contentLength = 0;
	responseData << "Content-Length: " << _contentLength << "\r\n";
	responseData << "Date: " << util::getDate() << "\r\n\r\n";
	if (_stateCode == 400 || _stateCode == 404 || _stateCode == 500 || _stateCode == 413)
		responseData << body;
	_responseStr = responseData.str();
}

void Client::handleError(int errnum)
{
	std::ifstream       file;
	std::string         filePath;
	std::stringstream   body;

	_stateCode = errnum;
	switch (errnum) {
		case 400:
			_reasonPhrase = "Bad Request"; break;
		case 404:
			_reasonPhrase = "Not Found"; break;
		case 405:
			_reasonPhrase = "Method Not Allowed"; break;
		case 413:
			_reasonPhrase = "Content Too Large"; break;
		case 500:
			_reasonPhrase = "Internal Server Error"; break;
		case 505:
			_reasonPhrase = "HTTP Version Not Supported"; break;
	}
	if (errnum < 500)
		filePath = "./src/pages/error/4XX.html";
	else
		filePath = "./src/pages/error/5XX.html";
	file.open(filePath.c_str());
	if (file.is_open())
	{
		body << file.rdbuf();
		_contentLength += body.str().length();
		pushErrorBuffer(body.str(), _stateCode);
	}
	file.close();
}