#pragma once

#include "Server.hpp"
#include <iostream>
#include <string>
#include <stack>
#include <fstream>

class Operation;

class Configuration
{
	private:
		enum E_SERVER_KEY
		{
			NAME = 0,
			LISTEN,
			ERROR,
			INDEX,
			MAXBODYSIZE,
		};
		enum E_LOCATION_KEY
		{
			PATH = 0,
			ROOT,
			L_INDEX,
			AUTOINDEX,
			UPLOAD,
			PY,
			PHP,
			CLIENT_MAX_BODY_SIZE,
			LIMIT_EXCEPT,
			TRY_FILES
		};
		enum E_KEY_VALUE
		{
			KEY = 0,
			VALUE
		};
		Operation&	_operation;
		std::stack<std::string> _parenticts;
		bool                    _locationFlag;
		bool                    _serverFlag;
		unsigned int            _blockCount;

		Configuration(const Configuration& other); // Copy Constructor
		Configuration& operator=(const Configuration& other); // Assignment Operator
		// Configuration(); // Default Constructor
	public:
		Configuration(Operation& operation);// operation Constructor;
		~Configuration(); // Destructor

		void parsing(const std::string& filePath);
		void push(const std::string& input);
		void pop(Server& server, Location& location);
		void setConfigValue(const std::string& key, const std::string& value, const std::string& line, Server& server, Location& location);	
};
