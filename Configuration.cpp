#include "Configuration.hpp"
#include <sstream> // 헤더 추가

void Configuration::parsing(const std::string& filePath)
{
    std::ifstream	file;
	std::vector<Server> servers;	

    file.open(filePath);
    while(file.eof() == false) 
    {
        std::string line; 
        getline(file, line);
        //split line for space

        std::istringstream iss(line); // istringstream을 사용하여 문자열을 공백으로 자름
        std::string word;
        while (iss >> word) // 단어마다 반복
        {
            if (word.empty() == true)
                break;
            else if (word == '{')
                setStack(push);
            else if (word == '}')
                setStack(pop);
            // 이제 단어를 사용하여 필요한 작업을 수행할 수 있습니다.
            // 예를 들어, 단어를 벡터에 저장하거나 다른 처리를 할 수 있습니다.
        }
    }
    file.close();
}

