#include "Server.hpp"
#include "Configuration.hpp"
#include "Operation.hpp"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    // if (argc != 2)
    // {
    //     std::cout << "Error: Invalid number of arguments" << std::endl;
    //     return 1;
    // }
    try
    {
        Operation operation;
        Configuration config(operation);
    std::string filename = "conf/default.conf";
        config.parsing(filename); 
    } 
    catch(std::exception &e) 
    {
        std::cout << e.what() << std::endl;
        return 1;
    }

//////////test line ///////////

    // std::cout << operation._servers[0].getServerName(0) << std::endl;
    // std::cout << operation._servers[0].getErrorPage(0) << std::endl;
    // std::cout << operation._servers[0].getListen() << std::endl;

    // operation.start(); return 0;
}