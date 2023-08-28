#include "Server.hpp"
#include "Configuration.hpp"
#include "Operation.hpp"

int main(int argc, char **argv)
{
    if (argc > 2)
    {
        std::cerr << "Error: Invalid number of arguments" << std::endl;
        return EXIT_ERROR;
    }
    std::string filename = "conf/default.conf";
    if (argc == 2)
        filename = argv[1];
    Operation operation;
    Configuration config(operation);
    try
    {
        config.parsing(filename);
        operation.start();
    }
    catch(int errnum)
    {
        std::cerr << "Main errnum catch >> " << errnum << std::endl;
        operation.start();
    }
    catch(std::exception &e) 
    {
        std::cerr << "Main catch >> " << e.what() << std::endl;
        return EXIT_ERROR;
    }
    return EXIT_DONE;
}