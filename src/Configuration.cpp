#include "Configuration.hpp"

Configuration::Configuration(Operation& operation) 
: _operation(operation), _tokenState(state::SERVER), _stackState(0), _blockCount(0){}

void Configuration::parsing(const std::string& filePath)
{
	std::vector<std::string> token = getVectorLine(filePath);
	int	size = token.size();
	int checkList[size];
	memset(checkList, 0, sizeof(checkList));

	setCheckList(token, checkList);
	checkSyntax(checkList, size);
	checkSameKey(token, checkList);
	setValue(token, checkList);
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
	if (totalLine.empty())
	{	
		file.close();
		throw std::logic_error("Error: File size error");
	}
	token = getToken(totalLine, "\t\r\v\n {};");
	file.close();
	return token;
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
	int prev = 0;
	int cur;

	for(int i = 0; i < size; ++i)
	{ 
		cur = checkList[i];
		if (prev == cur && !(prev == token::PATH || prev == token::CLOSE_BRACKET || prev == token::VALUE))
			throw std::logic_error("Error: Token Duplicate error");
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

void Configuration::checkSameKey(std::vector<std::string> &token, int *checklist)
{ 
	int state = state::SERVER;
	int index;
	int serverTable[server::SIZE];
	int locationTable[location::SIZE];

	for (size_t i = 0; i < token.size(); ++i)
	{
		if (checklist[i] == token::SERVER)
		{
			state = state::SERVER;
			memset(serverTable, 0, sizeof(serverTable));
		}
		else if (checklist[i] == token::LOCATION)
		{
			state = state::LOCATION;
			memset(locationTable, 0, sizeof(locationTable));
		}
		else if (checklist[i] == token::KEY)
		{
			if (state == state::SERVER)
			{
				index = findServerKey(token[i]);
				if (index == -1)
					throw std::logic_error("Error: Invalid key");
				if (serverTable[index] > 0)
					throw std::logic_error("Error: Same Server Key error");
				serverTable[index] = 1;
			}
			else if (state == state::LOCATION)
			{
				index = findLocationKey(token[i]);
				if (index == -1)
					throw std::logic_error("Error: Invalid key");
				if (locationTable[index] > 0)
					throw std::logic_error("Error: Same Location Key error");
				locationTable[index] = 1;
			}
		}
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
			_operation.setServer(server);
		else if (checklist[i] == token::CLOSE_BRACKET && state == state::LOCATION)
		{
			server.setLocation(location);
			state = state::SERVER;
		}
		if (checklist[i] == token::SERVER)
		{
			state = state::SERVER;
			Server newServer;
			server = newServer;
		}
		else if (checklist[i] == token::LOCATION)
		{
			state = state::LOCATION;
			Location newlocation;
			location = newlocation;
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
				i += 1;
				while (checklist[i] != token::SEMICOLON)
				{
					setLocationValue(location, index, token[i]);
					++i;
				}
			}
		}
		else if (checklist[i] == token::PATH)
		{
			location._path = token[i];
		}
	}
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
	std::string locationDirective[] =
	{"root", "index", "autoindex", "upload", ".py", ".bla", ".php", "client_max_body_size", "limit_except","try_files"};
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
		case location::BLA:
			return (location::BLA);
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
        case location::BLA:
			location._bla = value; break;
		case location::PHP:
			location._php = value; break;
		case location::CLIENT_MAX_BODY_SIZE:
			location._clientMaxBodySize = value; break;
		case location::LIMIT_EXCEPT:
			location._limitExcept.push_back(value); break;
		case location::TRY_FILES:
			location._tryFiles = value; break;
	}
}

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
	if (top != token::OPEN_BRACKET)
		throw std::logic_error("Error: { is not exist");
	_bracket.pop();
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
