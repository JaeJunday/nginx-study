#pragma once

#include "Server.hpp"
#include "Operation.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <stack>
#include <vector>
#include <map>

class Operation;
class Configuration
{
	private:
		Operation&		_operation;
		std::stack<int>	_bracket;
		int 			_tokenState;
		int 			_stackState;
		unsigned int	_blockCount;

		Configuration(const Configuration& other);
		Configuration& operator=(const Configuration& other);
		Configuration();
	public:
		Configuration(Operation& operation);
		~Configuration();

		void parsing(const std::string& filePath);
		std::vector<std::string> getVectorLine(const std::string& filePath) const;
		std::vector<std::string> getToken(std::string& str, const std::string& delimiters) const;
		void setCheckList(std::vector<std::string> &vectorLine, int *checklist);
		void checkSyntax(int *checkList, int size);
		void checkSameKey(std::vector<std::string> &token, int *checklist);
		void checkSamePath();
		//
		void setValue(std::vector<std::string> &token, int *checklist);
		int	 findLocationKey(const std::string& token) const;
		int	 findServerKey(const std::string& token) const;
		void push(int input);
		void pop();
		void setConfigValue(const std::string& key, const std::string& value, Server& server, Location& location);
		void setLocationValue(Location& location, int index, std::string& value);
};
