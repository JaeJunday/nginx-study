#include "Util.hpp"

std::vector<std::string> util::getToken(const std::string& str, const std::string& delimiters)
{
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = 0;

    while (end != std::string::npos) {
        end = str.find(delimiters, start);
        if (end != start) 
        {
            std::string tmp = str.substr(start, (end == std::string::npos) ? std::string::npos : end - start);
            if (tmp.empty() == false)
                result.push_back(tmp);
        }
        if (end == std::string::npos) 
            break;
        start = end + 1;
    }
    return result;
}

uint32_t util::stoui(const std::string& str)
{
    return static_cast<uint32_t>(std::strtod(str.c_str(), NULL));
}

std::string util::getDate()
{
    std::time_t now = std::time(NULL);
    char timeStamp[TIME_SIZE];
    std::strftime(timeStamp, sizeof(timeStamp), "%H:%M:%S GMT", std::localtime(&now));
    return (timeStamp);
}

std::string util::findContentType(const std::string& filePath)
{
	std::vector<std::string> filename = util::getToken(filePath, ".");
	std::string fileExtension;
	size_t lastElement = filename.size();
    if (lastElement >= 1)
		fileExtension = filename[filename.size() - 1];
	else
		return "text/plain";

	std::string fileType[] = {"html", "css", "js", "json", "jpeg", "jpg", "png", "gif", "bmp", "webp", "mpeg", "wav", "ogg", "mp4", "webm", "pdf", "zip", "csv"};
    std::string inputType[] = {"text/html", "text/css", "text/javascript", "application/json", "image/jpeg", "image/jpeg", "image/png", "image/gif", "image/bmp", "image/webp", "audio/mpeg", "audio/wav", "audio/ogg", "video/mp4", "video/webm", "application/pdf", "application/zip", "text/csv"};

	for (size_t i = 0; i < sizeof(fileType) / sizeof(std::string); ++i)
	{
		if (fileExtension == fileType[i])
			return inputType[i];
	}
	return "";
}

void util::checkMultipleSpaces(const std::string& str)
{
	char prev = '\0';
	for (size_t i = 0; i < str.size(); ++i) {
		if ((prev == ' ' || prev == '\n') && prev == str[i])
			throw 400;
		prev = str[i];
	}
}
