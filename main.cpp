#include "Server.hpp"
#include "Configuration.hpp"
#include "Operation.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "Error: Invalid number of arguments" << std::endl;
        return 1;
    }
    Operation operation;
    Configuration config(operation);
    config.parsing(argv[1]);

//////////test line ///////////

    std::cout << operation._servers[0].getServerName(0) << std::endl;
    std::cout << operation._servers[0].getErrorPage(0) << std::endl;
    std::cout << operation._servers[0].getListen() << std::endl;

    operation.start();
    return 0;
}