#include "Request.hpp"

void Request::setServer(Server* server)
{
	_server = server;
}

void Request::setRequestLine(std::string& requestLine)
{
	util::checkMultipleSpaces(requestLine);
	std::vector<std::string> token = util::getToken(requestLine, " ");

	if (token.size() != 3)
		throw 400;
	if (!(token[0] == "GET" || token[0] == "DELETE" || token[0] == "POST" || token[0] == "PUT"))
		throw 405;
	if (token[2] != "HTTP/1.1")
		throw 505;
	_method = token[0];
	_requestPath = token[1];
	_version = token[2];
}

void Request::setFieldLine(std::string& fieldLine)
{
	util::checkMultipleSpaces(fieldLine);
	std::vector<std::string> token = util::getToken(fieldLine, ": ");
	token[1].erase(0, 1);
	if (token.size() != 2)
		throw 400;
	if (token[0].empty() || token[1].empty())
		throw 400;
	if (token[0].find(' ') != std::string::npos)
		throw 400;
	if (token[0] == "Host") 
	{
		size_t mid = token[1].find(":");
		if (mid == std::string::npos)
		{
			token[1] += ":80";
			mid = token[1].find(":");
		}
		_host = std::string(token[1], 0, mid);
		_port = util::stoui(std::string(token[1], mid + 1, token[1].size() - (mid + 1)));
	}
	if (token[0] == "Content-Type")
	{
		const std::string hash = "boundary=";
		size_t index = token[1].find(hash);
		if (index != std::string::npos)
			_boundary = "--" + std::string(token[1], index + hash.length(), token[1].size() - (index + hash.length()));
		_contentType = token[1];
	} 
	if (token[0] == "Content-Length")
		_contentLength = token[1];
	if (token[0] == "Transfer-Encoding")
		_transferEncoding = token[1];
	if (token[0] == "Connection")
		_connection = token[1];
	if (token[0] == "X-Secret-Header-For-Test")
		_secretHeader = token[1];
}

void Request::setRequestBody(std::string& body)
{
	_requestBody = body;
}

void Request::setBodyTotalSize(int bodyTotalSize)
{
	_bodyTotalSize = bodyTotalSize;
}

void Request::setChunkedEnd(bool set)
{
	_chunkedEnd = set;
}

