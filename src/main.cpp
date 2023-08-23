#include "Server.hpp"
#include "Configuration.hpp"
#include "Operation.hpp"
#include <cstdlib>

int main(int argc, char **argv)
{
    std::string filename = "conf/default.copy.conf";
    if (!(argc == 1 || argc == 2))
    {
        std::cerr << "Error: Invalid number of arguments" << std::endl;
        return 1;
    }
    if (argc == 2)
        filename = argv[1];
    Operation operation;
    Configuration config(operation);
    try
    {
        config.parsing(filename);
        operation.start();
    } 
    catch(std::exception &e) 
    {
        std::cerr << "Main catch >> " << e.what() << std::endl;
        // operation.start();
        // exit(0);
        // return EXIT_FAILURE;
    }
    return 0;
}