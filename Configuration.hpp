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
		Operation&		_operation;
		std::stack<int>	_bracket;
		int 			_tokenState;
		// bool			_locationFlag;
		// bool			_serverFlag;
		// bool			_pathFlag;
		int 			_stackState;
		unsigned int	_blockCount;
		int				_serverTable[server::SIZE];
		int				_locationTable[location::SIZE];

		Configuration(const Configuration& other); // Copy Constructor
		Configuration& operator=(const Configuration& other); // Assignment Operator
		// Configuration(); // Default Constructor
	public:
		Configuration(Operation& operation);// operation Constructor;
		~Configuration(); // Destructor

		void checkSamePath();
	void setValue(std::vector<std::string> &token, int *checklist);
		int	 findServerKey(const std::string& token) const;
		int	 findLocationKey(const std::string& token) const;
		void checkSameKey(std::vector<std::string> &token, int *checklist);
		void checkSyntax(int *checkList, int size);
		void parsing(const std::string& filePath);
		void push(int input);
		void pop();
		void setCheckList(std::vector<std::string> &vectorLine, int *checklist);
		void setConfigValue(const std::string& key, const std::string& value, Server& server, Location& location);
		void setLocationValue(Location& location, int index, std::string& value);
		std::vector<std::string> getVectorLine(const std::string& filePath) const;
		std::vector<std::string> getToken(std::string& str, const std::string& delimiters) const;
};
