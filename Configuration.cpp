#include "Configuration.hpp"
#include "Server.hpp"
#include "Operation.hpp"
#include <cstddef>
#include <sstream> // 헤더 추가
#include <stdexcept>
#include <string>
#include <tuple>
// OCF

// Configuration::Configuration() : _operation(NULL), _locationFlag(false), _serverFlag(false), _blockCount(0)
// {
//     // Default Constructor Implementation
// }

Configuration::Configuration(Operation& operation) 
: _operation(operation), _status(false), _locationFlag(false), _serverFlag(false), _pathFlag(false), _blockCount(0)
{
}

Configuration::~Configuration()
{
    // Destructor Implementation
}

Configuration::Configuration(const Configuration& other)
    : _operation(other._operation),
      _bracket(other._bracket),
      _status(other._status),
      _locationFlag(other._locationFlag),
      _serverFlag(other._serverFlag),
      _pathFlag(other._pathFlag),
      _blockCount(other._blockCount)
{
    // Copy Constructor Implementation
}

Configuration& Configuration::operator=(const Configuration& other)
{
    if (this != &other) {
        _status = other._status;
        _locationFlag = other._locationFlag;
        _serverFlag = other._serverFlag;
        _pathFlag = other._pathFlag;
        _blockCount = other._blockCount;
        _bracket = other._bracket;
    }
    // Assignment Operator Implementation
    return *this;
}
//OCF =============================================================================================

static void test_printVector(const std::vector<std::string> &vectorLine)
{
    for (size_t i = 0; i < vectorLine.size(); i++)
        std::cout << vectorLine[i] << std::endl;
}

std::vector<std::string> Configuration::getVectorLine(const std::string& filePath) const
{
    std::ifstream	file;

    file.open(filePath);
    if (file.is_open() == false)
        throw std::logic_error("Error: File is not exist");
    std::vector<std::string> vectorLine;
    std::string totalLine;
    std::string line;

    while (file.eof() == false)
    {
        getline(file, line);
        totalLine += line;
    } 
    vectorLine = getToken(totalLine, "\t\r\v\n {};");
    file.close();
    // test_printVector(vectorLine); // To be deleted - kyeonkim
    return vectorLine;
}

// void setCheckList(vectorLine, checkList)
// {

// }

void Configuration::parsing(const std::string& filePath)
{
    std::vector<std::string> vectorLine = getVectorLine(filePath);
    Server server;
    Location location;
    int checkList[vectorLine.size()]; 
    memset(checkList, 0, sizeof(int));

    // checkList = setCheckList(vectorLine, checkList);
    // checkDupdirective(vectorLine, checkList);
    // chcekSemicolon(vectorLine, checkList);
    
    
   
    /* To be deleted - kyeonkim
    // while(file.eof() == false) 
    // {
    //     std::string line;
    //     getline(file, line);        
    //     std::vector<std::string> token = getToken(line, " {};");
    //     for (size_t i = 0; i < token.size(); i++)
    //     {
    //         if (token[i].empty() == true)
    //             continue;
    //         else if (_pathFlag == true && token[i] != "{" && token[i] != "}")
    //             location._path += token[i];
    //         else if (token[i] == "server" || token[i] == "location" || token[i] == "{")
    //             push(token[i]);
    //         else if (token[i] == "}")
    //             pop(server, location);
    //         else if (i >= VALUE)
    //             setConfigValue(token[KEY], token[i], server, location);
    //     }
    // }
    */
}

std::vector<std::string> Configuration::getToken(std::string& str, const std::string& delimiters) const 
{
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = 0;

    while (end != std::string::npos) {
        end = str.find(delimiters, start);
        if (end != start) 
            result.push_back(str.substr(start, (end == std::string::npos) ? std::string::npos : end - start));
        if (end == std::string::npos) 
            break;
        if (str[end] == '{' || str[end] == '}' || str[end] == ';')
            result.push_back(std::string(str, end, 1));
        start = end + 1;
    }
    return result;
}


// std::vector<std::string> Configuration::getToken(const std::string& line, std::string seq)
// {
//     std::vector<std::string> result;
//     std::string tmp;

//     for(size_t i = 0; i < line.size(); i++)
//     {
//         //구분자일때
//         if (seq.find(line[i]) >= 0)
//         {
//             result.push_back(std::string(line, i, 1));
//             continue;
//         }
//         /////////////////////

//         bool flag = 0;
//         if (isspace(line[i]) && flag == true)
//         {
//             result.push_back(tmp);
//             flag = 0;
//         }
//         else if (!isspace(line[i]))
//         {
//             tmp += line[i];
//             flag = 1;
//         }
//     }
//     return result;
// }


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
        if (_bracket.top() == "server")
            _serverFlag = true;
        else if (_bracket.top() == "location")
        {
            _locationFlag = true;   
            _pathFlag = false;
        }
        else
            throw std::logic_error("Error: { is not pair");
    }
    _bracket.push(input);
}

void Configuration::pop(Server& server, Location& location) 
{
    std::string str;

    if (_bracket.empty() == true)
        throw std::logic_error("Error: } is not pair");
    // first pop
    str = _bracket.top();
    if (str != "{")
        throw std::logic_error("Error: { is not exist");
    _bracket.pop();
    // second pop
    str = _bracket.top();
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
    _blockCount -= 1;
    _bracket.pop();
}


void Configuration::setConfigValue(const std::string& key, const std::string& value, Server& server, Location& location)
{
    size_t i;
    size_t length;

    if (_serverFlag == true && _locationFlag == true)
    {
        // set location
        static std::string locationDirective[] =
        {
        "root", "index", "autoindex", "upload", 
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
            // case PATH:
            //     location._path = value; break;
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
        std::cout << "here!!!" << std::endl;
        std::cout <<"key : " << key << ", value : " << value << std::endl;
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
