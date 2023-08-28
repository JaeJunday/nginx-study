#include "Request.hpp"

void Request::parsingChunkedData()
{
	if (_readIndex == false)
		_readIndex = _bodyStartIndex;
	size_t requestBufferSize = _requestBuffer.size();
	char* requestBuffer = const_cast<char *>(_requestBuffer.c_str());

	while (true)
	{
		if (parseChunkedBody(requestBuffer, requestBufferSize) == false)
			break;
	}
}

bool Request::parseChunkedBody(char* requestBuffer, size_t requestBufferSize)
{
	char* endptr;
	size_t bodySize = std::strtol(requestBuffer + _readIndex, &endptr, HEX);

	if (!(endptr[0] == '\r' && endptr[1] == '\n'))
		return false;
	size_t bodyStart = (endptr - requestBuffer) + 2;
	if (bodySize == 0)
		handleChunkedEnd(bodyStart);
	else if (bodyStart + bodySize + 2 <= requestBufferSize)
	{
		appendChunkedData(requestBuffer, bodyStart, bodySize);
		return true;
	}
	return false;
}

void Request::handleChunkedEnd(size_t bodyStart)
{
	if (_requestBuffer.find("\r\n", bodyStart) != std::string::npos)
	{
		if (!_location._clientMaxBodySize.empty())
		{
			if (_requestBody.size() > util::stoui(_location._clientMaxBodySize))
				throw 413;
		}
		_chunkedEnd = true;
	}
}

void Request::appendChunkedData(char* requestBuffer, size_t bodyStart, size_t bodySize)
{
	if (_requestBuffer.find("\r\n", bodyStart + bodySize) != bodyStart + bodySize)
		throw 400;
	_requestBody.append(requestBuffer + bodyStart, bodySize);
	_bodyTotalSize = _requestBody.size();
	_readIndex = bodyStart + bodySize + 2;
}
