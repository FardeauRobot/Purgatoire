#include <iostream>
#include <fstream>
#include "utils/errors.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
        return (F_ErrMsg("Usage: ./PROJECT_NAME <filename>"));

    std::ifstream file(argv[1]);
    if (!file)
        return (F_ErrMsg(std::string("Cannot open file: ") + argv[1]));

    std::cout << "PROJECT_NAME: file opened successfully" << std::endl;
    return (SUCCESS);
}
