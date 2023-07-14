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

//OCF ==============================================================================

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
        std::string configKey;
        for (int i = 0; iss >> word; i++) // 단어마다 반복
        {
            if (word.empty() == true)
                break;
            else if (word == "server" || word == "location" || word == "{")
                push(word); 
            else if (word == "}")
                pop();
            else if (i == KEY)
                configKey = word;
            else if (i >= VALUE)
                setConfigValue(configKey, word, line);
                // 세미콜론은 값을 넣을때 같이 처리한다 한번만 line검사하면 됨
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
    if (input == "server")
    {
        if (_serverFlag == true || _locationFlag == true)
            throw std::logic_error("Error: Server is already exist");
        ++_count;
    }
    if (input == "location")
    {
        if (_locationFlag == true || _serverFlag == false)
            throw std::logic_error("Error: Location is already exist");
        _locationFlag = true;
        ++_count;
    }
    if (input == "{")
    {
        if (_parenticts.top() == "server")
            _serverFlag = true;
        else if (_parenticts.top() == "location")
            _locationFlag = true;   
    }
    _parenticts.push(input);
}


void Configuration::setConfigValue(const std::string& key, const std::string& value)
{
    static std::string serverDirective[] = {
        "server_name", "listen", "error_page", "index", 
        "client_max_body_size"
    };
    size_t i;
    size_t length = sizeof(serverDirective) / sizeof(std::string);
    
    for (i = 0; i < length; i++)
    {
        if (key == serverDirective[i])
            break;
    }
    if (i == length)
        throw std::logic_error("Error: Invalid key");
    switch (i)
    {
        case NAME:
        {
            
        }
        case LISTEN:
        case ERROR:
        case INDEX:
        case MAXBODYSIZE:
        default:
    }
}