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
			ROOT = 0,
			L_INDEX,
			AUTOINDEX,
			UPLOAD,
			PY,
			PHP,
			CLIENT_MAX_BODY_SIZE,
			LIMIT_EXCEPT,
			TRY_FILES
		};
		enum E_CHECK_TOKEN
		{
			SERVER = 1,
			LOCATION,
			PATH,
			BRACKET,
			SEMI,
			KEY,
			VALUE
		}
		Operation&	_operation;
		std::stack<std::string> _parenticts;
		int 					_status;
		bool                    _locationFlag;
		bool                    _serverFlag;
		bool					_pathFlag;
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
		void setConfigValue(const std::string& key, const std::string& value, Server& server, Location& location);	
		std::vector<std::string> getVectorLine() const;
		std::vector<std::string> getToken(const std::string& str, const std::string& delimiters);
};
