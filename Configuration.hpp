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
		Operation&	_operation;
		std::stack<std::string> _bracket;
		int 					_state;
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
		void setCheckList(std::vector<std::string> &vectorLine, int *checklist);
		void setConfigValue(const std::string& key, const std::string& value, Server& server, Location& location);	
		std::vector<std::string> getVectorLine(const std::string& filePath) const;
		std::vector<std::string> getToken(std::string& str, const std::string& delimiters) const;
};
