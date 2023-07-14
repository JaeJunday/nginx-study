#include "Server.hpp"
#include "Configuration.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "Error: Invalid number of arguments" << std::endl;
        return 1;
    }
	std::vector<Server> servers;
    Configuration config;
    config.parsing(argv[1]);
    return 0;
}