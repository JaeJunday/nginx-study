#include "Configuration.hpp"
#include <sstream> // 헤더 추가

// OCF

Configuration::Configuration() : _locationFlag(false), _serverFlag(false), _count(0)
{
    // Default Constructor Implementation
}

Configuration::~Configuration()
{
    // Destructor Implementation
}

Configuration::Configuration(const Configuration& other)
    : _locationFlag(other._locationFlag),
      _serverFlag(other._serverFlag),
      _count(other._count),
      _parenticts(other._parenticts)
{
    // Copy Constructor Implementation
}

Configuration& Configuration::operator=(const Configuration& other)
{
    if (this != &other) {
        _locationFlag = other._locationFlag;
        _serverFlag = other._serverFlag;
        _count = other._count;
        _parenticts = other._parenticts;
    }
    // Assignment Operator Implementation
    return *this;
}

//==============================================================================

void Configuration::parsing(const std::string& filePath)
{
    std::ifstream	file;
	std::vector<Server> servers;	

    file.open(filePath);
    while(file.eof() == false) 
    {
        std::string line; 
        getline(file, line);
        //split line for space

        std::istringstream iss(line); // istringstream을 사용하여 문자열을 공백으로 자름
        std::string word;
        while (iss >> word) // 단어마다 반복
        {
            if (word.empty() == true)
                break;
            // refactoring 예정
            else if (word == "server")
            {
                if (_serverFlag == true)
                    throw std::runtime_error("Error: Server is already exist");
                else if (_locationFlag == true)
                    throw std::runtime_error("Error: Location is already exist");
                else
                {
                    _serverFlag = true;
                    ++_count;
                    push("server");
                }
            }
            else if (word == "location")
            {
                if (_locationFlag == true)
                    throw std::runtime_error("Error: Location is already exist");
                else if (_serverFlag == false)
                    throw std::runtime_error("Error: Server is not exist");
                else
                {
                    _locationFlag = true;
                    ++_count;
                    push("location");
                }
            }
            else if (word == "{")
                push("{"); 
            else if (word == "}")
            {
                pop();
            }
            // 이제 단어를 사용하여 필요한 작업을 수행할 수 있습니다.
            // 예를 들어, 단어를 벡터에 저장하거나 다른 처리를 할 수 있습니다.
        }
    }
    file.close();
}

void Configuration::pop() 
{
    std::string str;

    if (_parenticts.empty() == true)
        throw std::logic_error("Error: } is not pair");
    // first pop
    str = _parenticts.top();
    if (str != "{")
        throw std::logic_error("Error: { is not exist");
    _parenticts.pop();
    // second pop
    str = _parenticts.top();
    if (str != "server" && str != "location")
        throw std::logic_error("Error: server or location is not exist");
    if (str == "server")
        _serverFlag = false;
    else if (str == "location")
        _locationFlag = false;
    _count -= 1;
    _parenticts.pop();
}

void Configuration::push(const std::string& input) 
{

}

