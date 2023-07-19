#include "Configuration.hpp"
#include "Server.hpp"
#include "Operation.hpp"
#include <cstddef>
#include <sstream> // 헤더 추가
#include <stdexcept>
#include <string>
#include <iomanip>
#include "enum.hpp"
// OCF

// Configuration::Configuration() : _operation(NULL), _locationFlag(false), _serverFlag(false), _blockCount(0)
// {
//     // Default Constructor Implementation
// }

Configuration::Configuration(Operation& operation) 
: _operation(operation), _tokenState(state::SERVER), _stackState(0), _blockCount(0)
{
    memset(_serverTable, 0, sizeof(_serverTable));
    memset(_locationTable, 0, sizeof(_locationTable));
}

Configuration::~Configuration()
{
    // Destructor Implementation
}

Configuration::Configuration(const Configuration& other)
    : _operation(other._operation),
      _bracket(other._bracket),
      _tokenState(other._tokenState),
	  _stackState(other._stackState),
      _blockCount(other._blockCount)
{
    // Copy Constructor Implementation
}

Configuration& Configuration::operator=(const Configuration& other)
{
    if (this != &other) {
        // _status = other._status;
        _tokenState = other._tokenState;
		_stackState = other._stackState;
        _blockCount = other._blockCount;
        _bracket = other._bracket;
    }
    // Assignment Operator Implementation
    return *this;
}
//OCF =============================================================================================

static void test_printVector(const std::vector<std::string> &token)
{
    std::cout << token.size() << std::endl;
    for (size_t i = 0; i < token.size(); ++i)
        std::cout << token[i] << std::endl;
}

static void test_printCheckList(int *checklist, size_t size)
{
    for (size_t i = 0; i < size; ++i)
        std::cout << checklist[i] << std::endl;
}

static void test_print(const std::vector<std::string> &token, int *checklist)
{
	std::cout << "| 1 SERVER | 2 LOCATION | 3 PATH | 4 OPEN_BRACKET |\n| 5 CLOSE_BRACKET | 6 SEMICOLON | 7 KEY | 8 VALUE |\n\n";
    for (size_t i = 0; i < token.size(); ++i)
        std::cout << checklist[i] << " === " << token[i] << std::endl;
}

std::vector<std::string> Configuration::getVectorLine(const std::string& filePath) const
{
    std::ifstream	file;

    file.open(filePath);
    if (file.is_open() == false)
        throw std::logic_error("Error: File is not exist");
    std::vector<std::string> token;
    std::string totalLine;
    std::string line;

    while (file.eof() == false)
    {
        getline(file, line);
        totalLine += line;
    } 
    token = getToken(totalLine, "\t\r\v\n {};");
    file.close();
    // test_printVector(token); // To be deleted - kyeonkim
    return token;
}

void Configuration::setCheckList(std::vector<std::string> &token, int *checklist)
{
	for (size_t i = 0; i < token.size(); ++i)
	{
		if (token[i] == "server")
        {
			checklist[i] = token::SERVER;
            _tokenState = state::SERVER;
        }
        else if (token[i] == "location" && (_tokenState == state::KEY || _tokenState == state::SEMICOLON || _tokenState == state::CLOSE_BRACKET))
        {
            checklist[i] = token::LOCATION;
            _tokenState = state::LOCATION;
        }
        else if (token[i] == "{" && (_tokenState == state::PATH || _tokenState == state::SERVER))
        {
			checklist[i] = token::OPEN_BRACKET;
            _tokenState = state::KEY;
        }
        else if (token[i] == "}")
		{
            if (_tokenState == state::CLOSE_BRACKET)
                _tokenState = state::SERVER;
            else if (_tokenState == state::SEMICOLON)
                _tokenState = state::CLOSE_BRACKET;
			checklist[i] = token::CLOSE_BRACKET;
		}
		else if (token[i] == ";" && _tokenState == state::VALUE)
		{
			checklist[i] = token::SEMICOLON;
            _tokenState = state::KEY;
		}
        else
        {
			if (_tokenState == state::PATH || _tokenState == state::LOCATION)
			{
				checklist[i] = token::PATH;
                _tokenState = state::PATH;
			}
            else if (token[i] != ";" && _tokenState == state::KEY)
            {
				checklist[i] = token::KEY;
                _tokenState = state::VALUE;
            }
            else if (_tokenState == state::VALUE)
				checklist[i] = token::VALUE;
            else
                throw std::logic_error("Error: Token is Invalid");
        }
	}
}

void Configuration::checkSyntax(int *checkList, int size)
{
    int prev = 1;
	int cur;
	
    for(int i = 0; i < size; ++i)
    { 
        cur = checkList[i];
        if (prev == cur && !(prev == token::PATH || prev == token::CLOSE_BRACKET || prev == token::VALUE))
            std::logic_error("Error: Token Duplicate error");
        if (prev == token::VALUE && (cur == token::LOCATION || cur == token::CLOSE_BRACKET))
            throw std::logic_error("Error: Token Semicolon error");
        if (prev == token::KEY && cur == token::SEMICOLON)
            throw std::logic_error("Error: Token Semicolon error");
        switch (cur)
        {
            case token::SERVER : 
				push(cur); break;
            case token::LOCATION :
                push(cur); break;
            case token::OPEN_BRACKET :
                push(cur); break;
            case token::CLOSE_BRACKET :
                pop(); break;
        }
        prev = checkList[i];
    }
    if (_blockCount != 0)
        throw std::logic_error("Error: Token Bracket not pair");
}

int Configuration::findServerKey(const std::string& key) const
{
    std::string serverDirective[] = {"server_name", "root", "listen", "error_page", "index", "client_max_body_size"};
    int res = -1;
    size_t i;
    size_t length = sizeof(serverDirective) / sizeof(std::string);
        
    for (i = 0; i < length; ++i)
    {
        if (key == serverDirective[i])
            break;
    }
    switch (i)
    {
        case server::NAME:
            return (server::NAME);
        case server::ROOT:
            return (server::ROOT);
        case server::LISTEN:
            return (server::LISTEN);
        case server::ERROR:
            return (server::ERROR);
        case server::INDEX:
            return (server::INDEX);
        case server::MAXBODYSIZE:
            return (server::MAXBODYSIZE);
    }
    return (res);
}

int Configuration::findLocationKey(const std::string& key) const
{
    static std::string locationDirective[] =
    {"root", "index", "autoindex", "upload", "py", "php", "client_max_body_size", "limit_except","try_files"};
    int res = -1;
    size_t i;
    size_t length = sizeof(locationDirective) / sizeof(std::string);
        
    for (i = 0; i < length; ++i)
    {
        if (key == locationDirective[i])
            break;
    }
    switch (i)
    {
        case location::ROOT:
            return (location::ROOT);
        case location::INDEX:
            return (location::INDEX);
        case location::AUTOINDEX:
            return (location::AUTOINDEX);
        case location::UPLOAD:
            return (location::UPLOAD);
        case location::PY:
            return (location::PY);
        case location::PHP:
            return (location::PHP);
        case location::CLIENT_MAX_BODY_SIZE:
            return (location::CLIENT_MAX_BODY_SIZE);
        case location::LIMIT_EXCEPT:
            return (location::LIMIT_EXCEPT);
        case location::TRY_FILES:
            return (location::TRY_FILES);
    }
    return (res);
}

void Configuration::checkSameKey(std::vector<std::string> &token, int *checklist)
{ 
    int state = state::SERVER;
    int index;
    
    for (size_t i = 0; i < token.size(); ++i)
    {
        if (checklist[i] == token::SERVER)
        {
            state = state::SERVER;
            memset(_serverTable, 0, sizeof(_serverTable));
        }
        else if (checklist[i] == token::LOCATION)
        {
            state = state::LOCATION;
            memset(_locationTable, 0, sizeof(_locationTable));
        }
        else if (checklist[i] == token::KEY)
        {
            if (state == state::SERVER)
            {
                index = findServerKey(token[i]);
                if (index == -1)
                    throw std::logic_error("Error: Invalid key");
                if (_serverTable[index] > 0)
                    throw std::logic_error("Error: Same Server Key error");
                _serverTable[index] = 1;
            }
            else if (state == state::LOCATION)
            {
                index = findLocationKey(token[i]);
                if (index == -1)
                    throw std::logic_error("Error: Invalid key");
                if (_locationTable[index] > 0)
                    throw std::logic_error("Error: Same Location Key error");
                _locationTable[index] = 1;
            }
        }
    }
}

void Configuration::setLocationValue(Location& location, int index, std::string& value)
{
    switch (index)
    {
        case location::ROOT:
            location._root = value; break;
        case location::INDEX:
            location._index = value; break;
        case location::AUTOINDEX:
            location._autoindex = value; break;
        case location::UPLOAD:
            location._upload = value; break;
        case location::PY:
            location._py = value; break;
        case location::PHP:
            location._php = value; break;
        case location::CLIENT_MAX_BODY_SIZE:
            location._clientMaxBodySize = value; break;
        case location::LIMIT_EXCEPT:
            location._limitExcept = value; break;
        case location::TRY_FILES:
            location._tryFiles = value; break;
    }
}

void Configuration::setValue(std::vector<std::string> &token, int *checklist)
{
    int state = state::SERVER;
    int index;
    Server server;
    Location location;

    for (size_t i = 0; i < token.size(); ++i)
    {
        if (checklist[i] == token::CLOSE_BRACKET && state == state::SERVER)
            _operation._servers.push_back(server);
        else if (checklist[i] == token::CLOSE_BRACKET && state == state::LOCATION)
        {
            server.setLocation(location);
            state = state::SERVER;
        }
        if (checklist[i] == token::SERVER)
        {
            state = state::SERVER;
            memset(&server, 0, sizeof(Server));
        }
        else if (checklist[i] == token::LOCATION)
        {
            state = state::LOCATION;
            memset(&location, 0, sizeof(Location));
        }
        else if (checklist[i] == token::KEY)
        {
            if (state == state::SERVER)
            {
                index = findServerKey(token[i]);
                i += 1;
                while (checklist[i] != token::SEMICOLON)
                {
                    server.setValue(index, token[i]);
                    ++i;
                }
            }
            else if (state == state::LOCATION)
            {
                index = findLocationKey(token[i]);
                setLocationValue(location, index, token[++i]);
            }
        }
        else if (checklist[i] == token::PATH)
        {
            location._path = token[i];
        }

    }
}

void Configuration::checkSamePath()
{
    int size = _operation._servers.size();
    int locationSize;
    std::pair<std::map<std::string, char>::iterator, bool> result;
    std::string key;

    for (int i = 0; i < size; ++i)
    {
        std::map<std::string, char> checker;
        locationSize = _operation._servers[i].getLocationSize(); 
        for (int j = 0; j < locationSize; ++j)
        {
            key = _operation._servers[i].getLocation(j)._path;
            result = checker.insert(std::make_pair(key, '0')); 
            if (result.second == false)
                throw std::logic_error("Error: path duplicate");
        }
    }
}

void Configuration::parsing(const std::string& filePath)
{
    std::vector<std::string> token = getVectorLine(filePath);
	int	size = token.size();
    int checkList[size];
   memset(checkList, 0, sizeof(checkList));

    setCheckList(token, checkList);
	// test_print(token, checkList);
    checkSyntax(checkList, size);
    checkSameKey(token, checkList);
    setValue(token, checkList);
    checkSamePath();
    // setValue(token);
	// test_printCheckList(checkList, token.size());
    std::cout << "Done.\n" << std::endl;
   
    /* To be deleted - kyeonkim
    // while(file.eof() == false) 
    // { //     std::string line;
    //     getline(file, line);        
    //     std::vector<std::string> token = getToken(line, " {};");
    //     for (size_t i = 0; i < token.size(); ++i)
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
        end = str.find_first_of(delimiters, start);
        if (end != start) 
        {
            std::string tmp = str.substr(start, (end == std::string::npos) ? std::string::npos : end - start);
            if (tmp.empty() == false)
                result.push_back(tmp);
        }
        if (end == std::string::npos) 
            break;
        if (str[end] == '{' || str[end] == '}' || str[end] == ';')
            result.push_back(std::string(str, end, 1));
        start = end + 1;
    }
    return result;
}

// _stackState 초기값 : 0
void Configuration::push(int input)
{
    if (input == token::SERVER)
    {
		if (_stackState == stack::SERVER)
            throw std::logic_error("Error: Server is already exist");
		_stackState = stack::SERVER;
        ++_blockCount;
    }
    if (input == token::LOCATION)
    {
        if (_stackState == stack::LOCATION)
            throw std::logic_error("Error: Location is already exist");
		_stackState = stack::LOCATION;
        ++_blockCount;
    }
    _bracket.push(input);
}

void Configuration::pop() 
{
    if (_bracket.empty() == true)
        throw std::logic_error("Error: } is not pair");
    int top = _bracket.top();
    // first pop
    if (top != token::OPEN_BRACKET)
        throw std::logic_error("Error: { is not exist");
    _bracket.pop();

    // second pop
    top = _bracket.top();
    if (top != token::SERVER && top != token::LOCATION)
        throw std::logic_error("Error: server or location is not exist");
    if (top == token::SERVER)
		_stackState = 0;
    else if (top == token::LOCATION)
		_stackState = stack::SERVER;
    _blockCount -= 1;
    _bracket.pop();
}

/*
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
        
        for (i = 0; i < length; ++i)
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
        
        for (i = 0; i < length; ++i)
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
*/