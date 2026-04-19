#include <iostream>
#include "errors.hpp"

int main(int argc, char **argv)
{
    (void)argv;
    if (argc < 1)
        return (F_ErrMsg("invalid invocation"));

    std::cout << "PROJECT_NAME: hello world" << std::endl;
    return (SUCCESS);
}
