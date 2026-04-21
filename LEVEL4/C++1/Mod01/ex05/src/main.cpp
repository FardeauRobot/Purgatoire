#include <iostream>
#include "Errors.hpp"

#include "Harl.hpp"

int main(int argc, char **argv)
{
    Harl O_Main;

    if (argc <= 1)
        return (F_ErrMsg("invalid invocation"));

    std::string arg = argv[1];
    O_Main.complain(arg);

    return (SUCCESS);
}
