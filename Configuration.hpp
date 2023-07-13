#pragma once

#include "Server.hpp"
#include "Location.hpp"
#include <iostream>
#include <string>
#include <stack>
#include <fstream>

class Configuration
{
    std::stack<char> _parenticts;
    void parsing(const std::string& filePath);
};