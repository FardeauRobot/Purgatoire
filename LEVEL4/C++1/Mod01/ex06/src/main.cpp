#include "Errors.hpp"
#include "Harl.hpp"
#include <iostream>

int main(int argc, char **argv)
{

    if (argc != 2)
        return (F_ErrMsg("invalid invocation: ./harlFilter level"));

    Harl O_Main;
    std::string arg = argv[1];

    O_Main.complain(arg);

    return (SUCCESS);
}
