#include "Configuration.hpp"
#include "Server.hpp"
#include "Operation.hpp"
#include <cstddef>
#include <sstream> // 헤더 추가
#include <stdexcept>
#include <string>
// OCF

// Configuration::Configuration() : _operation(NULL), _locationFlag(false), _serverFlag(false), _blockCount(0)
// {
//     // Default Constructor Implementation
// }

Configuration::Configuration(Operation& operation) 
: _operation(operation), _locationFlag(false), _serverFlag(false), _pathFlag(false), _blockCount(0)
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
      _pathFlag(other._pathFlag),
      _blockCount(other._blockCount),
      _parenticts(other._parenticts)
{
    // Copy Constructor Implementation
}

Configuration& Configuration::operator=(const Configuration& other)
{
    if (this != &other) {
        _locationFlag = other._locationFlag;
        _serverFlag = other._serverFlag;
        _pathFlag(other._pathFlag),
        _blockCount = other._blockCount;
        _parenticts = other._parenticts;
    }
    // Assignment Operator Implementation
    return *this;
}
//OCF =============================================================================================

void Configuration::parsing(const std::string& filePath)
{
    std::ifstream	file;

    file.open(filePath);
    if (file.is_open() == false)
        throw std::logic_error("Error: File is not exist");
    Server server;
    Location location;
    while(file.eof() == false) 
    {
        // line : str str; str str str; str;
        std::string line; 
        getline(file, line);
        //line.find(";");

        //split line for space
        std::vector<std::string> token = getToken(line, "/r/t/n/v {}");
        for (int i = 0; i < token.size(); i++)
        {
            if (token[i].empty() == true)
                break;
            else if (token[i] == "server") //|| token[i] == "location" || token[i] == "{")
                push(token[i]);
            // else if (token[i] == "location")
            //     push(token[i]);
            // else if (token[i] == "{")
            //     push(token[i]
            else if (token[i] == "}")
                pop(server, location);
            else if (i >= VALUE)
                setConfigValue(token[KEY], token[i], server, location);
        }

        // std::istringstream iss(line); // istringstream을 사용하여 문자열을 공백으로 자름
        // std::string word;
        // std::string configKey;
        // for (int i = 0; iss >> word; i++) // 단어마다 반복
        // {
        //     if (word.empty() == true)
        //         break;
        //     else if (word == "server" || word == "location")// || word == "{")
//                push(token[i]);

        //     else if (word == "}")
        //         pop(server, location);
        //     else if (i == KEY)
        //         configKey = word;
        //     else if (i >= VALUE)
        //         setConfigValue(configKey, word, line, server, location);
        //         // 세미콜론은 값을 넣을때 같이 처리한다 한번만 line검사하면 됨
        // }
    }
    file.close();
}

std::vector<std::string> Configuration::getToken(const std::string& line, std::string seq)
{
    std::vector<std::string> result;
    std::string tmp = "";

// abc{}abcd acd
    const char *str = line.c_str();
    for(int i = 0; i < line.size(); i++)
    {
        if (str[i] == '{' || str[i] == '}')
        {
            result.push_back(std::string(str[i], 1));
            continue;
        }
        if (seq.find(str[i]) != std::string::npos) // 구분자가 없을 떈
            tmp += str[i];
        else // 구분자가 있을 땐
        {
            result.push_back(tmp);
            tmp = "";
        }
    }
    return result;
}

void Configuration::push(const std::string& input)
{
    if (input == "server")
    {
        if (_serverFlag == true || _locationFlag == true)
            throw std::logic_error("Error: Server is already exist");
        ++_blockCount;
    }
    if (input == "location")
    {
        if (_locationFlag == true || _serverFlag == false)
            throw std::logic_error("Error: Location is already exist");
        _pathFlag = true;
        ++_blockCount;
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
        _pathFlag = false;
    }
    _blockCount -= 1;
    _parenticts.pop();
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