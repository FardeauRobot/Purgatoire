#include <iostream>
#include <stdexcept>
#include "errors.hpp"

int main(int argc, char **argv)
{
    try
    {
        (void)argv;
        if (argc < 1)
            return (F_ErrMsg("invalid invocation"));

        std::cout << "PROJECT_NAME: hello world" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (FAILURE);
    }
    return (SUCCESS);
}
