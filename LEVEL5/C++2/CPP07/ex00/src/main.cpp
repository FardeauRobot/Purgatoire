#include <iostream>
#include <stdexcept>
#include "errors.hpp"
#include "utils.hpp"
#include "whatever.hpp"

int main(int argc, char **argv)
{
    try
    {
        (void)argv;
        if (argc < 1)
            return (F_ErrMsg("invalid invocation"));

        whatever instance("ex00");
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (FAILURE);
    }
    return (SUCCESS);
}
