#include <iostream>
#include <fstream>
#include <stdexcept>
#include "utils/errors.hpp"
#include "utils/utils.hpp"
#include "core/CLASS_NAME.hpp"

int main(int argc, char **argv)
{
    try
    {
        if (argc != 2)
            return (F_ErrMsg("Usage: ./EXEC_NAME <filename>"));

        std::ifstream file(argv[1]);
        if (!file)
            return (F_ErrMsg(std::string("Cannot open file: ") + argv[1]));

        CLASS_NAME instance("PROJECT_NAME");
        std::cout << "PROJECT_NAME: file opened successfully" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (FAILURE);
    }
    return (SUCCESS);
}
