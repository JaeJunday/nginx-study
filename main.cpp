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

    operation.start();
    return 0;
}