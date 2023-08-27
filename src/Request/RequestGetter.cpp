#include "Request.hpp"

const std::string& Request::getConnection() const
{
	return _connection;
}

const std::string& Request::getTransferEncoding() const
{
	return _transferEncoding;
}

const std::string& Request::getContentLength() const
{
	return _contentLength;
}

const std::string& Request::getHost() const
{
	return _host;
}

const std::string& Request::getMethod() const
{
	return _method;
}

const std::string& Request::getVersion() const
{
	return _version;
}

const std::string& Request::getConvertRequestPath() const
{
	return _convertRequestPath;
}

const std::string& Request::getRequestBuffer() const
{
	return _requestBuffer;
}

const Server* Request::getServer() const
{
	return _server;
}

const Location* Request::getLocation()
{
	return &_location;
}

const std::string& Request::getBoundary() const
{
	return _boundary;
}

const std::string& Request::getContentType()
{
	return _contentType;
}

int Request::getBodyIndex() const
{
	return _bodyStartIndex;
}

int Request::getChunkedEnd() const
{
	return _chunkedEnd;
}

int Request::getBodyTotalSize() const
{
	return _bodyTotalSize;
}

std::string& Request::getRequestBody()
{
	return _requestBody;
}

int Request::getBodyStartIndex() const
{
	return _bodyStartIndex;
}

const std::string& Request::getSecretHeader() const
{
	return _secretHeader;
}
