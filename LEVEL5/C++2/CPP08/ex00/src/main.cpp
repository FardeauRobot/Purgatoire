#include <iostream>
#include <stdexcept>
#include "Errors.hpp"
#include "utils.hpp"
#include "easyfind.hpp"

int main(int argc, char **argv)
{
    try
    {
        (void)argv;
        if (argc < 1)
            return (F_ErrMsg("invalid invocation"));

        easyfind instance("ex00");
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (FAILURE);
    }
    return (SUCCESS);
}
