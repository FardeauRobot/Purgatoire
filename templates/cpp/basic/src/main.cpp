#include <iostream>
#include <stdexcept>
#include "errors.hpp"
#include "utils.hpp"
#include "CLASS_NAME.hpp"

int main(int argc, char **argv)
{
    try
    {
        (void)argv;
        if (argc < 1)
            return (F_ErrMsg("invalid invocation"));

        CLASS_NAME instance("PROJECT_NAME");
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (FAILURE);
    }
    return (SUCCESS);
}
