#include "AResponse.hpp"
#include "Request.hpp"

AResponse::AResponse(int kq)
: _version("HTTP/1.1"), 
  _stateCode("200"), 
  _reasonPhrase("OK"),
  _serverName("My Server"),
  _contentType("text/html"),
  _contentLength(0),
  _kq(kq),
  _request(NULL)
{
    /* Constructor Implementation */
}

AResponse::AResponse(Request* request) : _request(request)
{
    /* Constructor Implementation */
}
AResponse::AResponse(const AResponse& src)
{
    /* Copy Constructor Implementation */
}

AResponse::~AResponse()
{
    /* Destructor Implementation */
}

AResponse& AResponse::operator=(AResponse const& rhs)
{
    if (this != &rhs)
    {
        /* Assignment Operator Implementation */
    }
    return *this;
}

std::string AResponse::getDate()
{
    std::time_t now = std::time(NULL);
    char timeStamp[TIME_SIZE];
    std::strftime(timeStamp, sizeof(timeStamp), "%a, %d %b %Y %H:%M:%S GMT", std::localtime(&now));
    return (timeStamp);
}

// 15:53:14 : 0.0.0.0 GET HTTP/1.1 200 Ok ./public 2259
void AResponse::stamp() const
{
    std::cerr << getDate() << " : "<< _request->getIp() << " " << _request->getMethod() << " " << _request->getVersion() << " "<< _stateCode << " " << _reasonPhrase << std::endl;
}

const std::stringstream& AResponse::getBuffer() const
{
    return _buffer;
}

std::string AResponse::findLocationPath() const
{
	const Server server = _request->getServer();
	const std::vector<Location>& locations = server.getLocations();
	std::string result = _request->getRequestUrl();
	Location location;
	int length = 0;
	if (result.empty())
	{
		// 경로가 없는 경우 errorcode
	}
	for (int i = 0; i < locations.size(); ++i) {
		int pathLength = locations[i]._path.length();
		if (_request->getRequestUrl().compare(0, pathLength, locations[i]._path) == 0)
		{
			if (length < pathLength)
			{
				length = pathLength;
				location = locations[i];
				_request->setLocation(const_cast<Location *>(&_request->getServer().getLocation(i)));
			}
		}
 	}
	if (length == false)
	{
		exit(1);
		// no location errorcode
	}
	checkLimitExcept();
	if (!location._root.empty())
	{
		result.erase(0, length);
		result = location._root + result;
	}
	else if (!server.getRoot().empty())	
	{
		result.erase(0, length);
		result = server.getRoot() + result;
	}
	if (result.size() > 1 && result[result.size() - 1] == '/')
		result.erase(result.size() - 1, 1);
		std::cerr << "result: " << result << std::endl;
	return result;
}

void AResponse::checkLimitExcept() const
{	
	std::string limit = _request->getLocation()->_limitExcept;
	if (!limit.empty())
	{	
		std::vector<std::string> allowMethod = util::getToken(limit, " ");
		int limitSize = allowMethod.size();
		if (limitSize)
		{
			int i = 0;
			while(i < limitSize)
			{
				if (allowMethod[i] == _request->getMethod())
					break;
				++i;
			}
			if (i == allowMethod.size())
				throw 405;
		}
	}
}

std::string AResponse::findContentType(const std::string& filePath)
{
	std::vector<std::string> filename = util::getToken(filePath, ".");
	std::string fileExtension;
	size_t lastElement = filename.size();
    if (lastElement >= 1)
		fileExtension = filename[filename.size() - 1];
	else
		return "text/plain";

	std::string fileType[] = {"html", "css", "js", "json", "jpeg", "jpg", "png", "gif", "bmp", "webp", "mpeg", "wav", "ogg", "mp4", "webm", "pdf", "zip"};
    std::string inputType[] = {"text/html", "text/css", "text/javascript", "application/json", "image/jpeg", "image/jpeg", "image/png", "image/gif", "image/bmp", "image/webp", "audio/mpeg", "audio/wav", "audio/ogg", "video/mp4", "video/webm", "application/pdf", "application/zip"};

	for (int i = 0; i < fileType->size(); ++i)
	{
		if (fileExtension == fileType[i])
			return inputType[i];
	}
	return "";
}
