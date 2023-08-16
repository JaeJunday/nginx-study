#include "Server.hpp"
#include "Configuration.hpp"
#include "Operation.hpp"
#include <cstdlib>

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    // if (argc != 2)
    // {
    //     std::cerr << "Error: Invalid number of arguments" << std::endl;
    //     return 1;
    // }
    // cerr << int << std::endl;

    Operation operation;
    Configuration config(operation);
    std::string filename = "conf/default.copy.conf";
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
    
//---------------------------------------------- testcode
    // std::cerr << operation._servers[0].getServerName(0) << std::endl;
    // std::cerr << operation._servers[0].getErrorPage(0) << std::endl;
    // std::cerr << operation._servers[0].getListen() << std::endl;
    return 0;
}