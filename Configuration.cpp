#include "Configuration.hpp"
#include "Server.hpp"
#include "Operation.hpp"
#include <sstream> // 헤더 추가
#include <stdexcept>
// OCF

// Configuration::Configuration() : _operation(NULL), _locationFlag(false), _serverFlag(false), _count(0)
// {
//     // Default Constructor Implementation
// }

Configuration::Configuration(Operation& operation) : _operation(operation), _locationFlag(false), _serverFlag(false), _count(0)
{
}

Configuration::~Configuration()
{
    // Destructor Implementation
}

Configuration::Configuration(const Configuration& other)
    : _operation(other._operation),
      _locationFlag(other._locationFlag),
      _serverFlag(other._serverFlag),
      _count(other._count),
      _parenticts(other._parenticts)
{
    // Copy Constructor Implementation
}

Configuration& Configuration::operator=(const Configuration& other)
{
    if (this != &other) {
        // _operation = other._operation; // const 여서 안됨 - kyeonkim
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
	// std::vector<Server> servers;

    file.open(filePath);
    Server server;
    Location location;
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
                pop(server, location);
            else if (i == KEY)
                configKey = word;
            else if (i >= VALUE)
                setConfigValue(configKey, word, line, server, location);
                // 세미콜론은 값을 넣을때 같이 처리한다 한번만 line검사하면 됨
        }
    }
    file.close();
}

void Configuration::pop(Server& server, Location& location) 
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
    {
        _operation.setServer(server);
        std::memset(&server, 0, sizeof(server));
        _serverFlag = false;
    }
    else if (str == "location")
    {
        server.setLocation(location);
        std::memset(&location, 0, sizeof(location));
        _locationFlag = false;
    }
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
        else
            throw std::logic_error("Error: { is not pair");
    }
    _parenticts.push(input);
}


void Configuration::setConfigValue(const std::string& key, const std::string& value, const std::string& line, Server& server, Location& location)
{
    size_t i;
    size_t length;

    if (_serverFlag == true && _locationFlag == true)
    {
        // set location
        static std::string locationDirective[] =
        {
        "path", "root", "index", "autoindex", "upload", 
        "py", "php", "client_max_body_size", "limit_except",
        "try_files"
        };
        length = sizeof(locationDirective) / sizeof(std::string);
        
        for (i = 0; i < length; i++)
        {
            if (key == locationDirective[i])
                break;
        }
        switch (i)
        {
            case PATH:
                location._path = value; break;
            case ROOT:
                location._root = value; break;
            case L_INDEX:
                location._index = value; break;
            case AUTOINDEX:
                location._autoindex = value; break;
            case UPLOAD:
                location._upload = value; break;
            case PY:
                location._py = value; break;
            case PHP:
                location._php = value; break;
            case CLIENT_MAX_BODY_SIZE:
                location._clientMaxBodySize = value; break;
            case LIMIT_EXCEPT:
                location._limitExcept = value; break;
            case TRY_FILES:
                location._tryFiles = value; break;
            default:
                throw std::logic_error("Error: Invalid key");
        }
    }
    else if (_serverFlag == true && _locationFlag == false)
    {
        static std::string serverDirective[] = {
            "server_name", "listen", "error_page", "index", 
            "client_max_body_size",   
        };
        size_t i;
        size_t length = sizeof(serverDirective) / sizeof(std::string);
        
        for (i = 0; i < length; i++)
        {
            if (key == serverDirective[i])
                break;
        }
        // if (i == length) // default 로 넣음 - kyeonkim
        //         throw std::logic_error("Error: Invalid key");
        switch (i)
        {
            case NAME:
                server.setServerName(value); break;
            case LISTEN:
                server.setListen(value); break;
            case ERROR:
                server.setErrorPage(value); break;
            case INDEX:
                server.setIndex(value); break;
            case MAXBODYSIZE:
                server.setClientMaxBodySize(value); break;
            default:
                throw std::logic_error("Error: Invalid key");
        }
    }
}