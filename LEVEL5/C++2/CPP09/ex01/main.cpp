#include <cctype>
#include <iostream>
#include <exception>
#include <sstream>
#include "errors.hpp"
#include "utils.hpp"
#include "RPN.hpp"

int main(int argc, char **argv)
{
    try
    {
        RPN instance;
        if (argc != 2)
            F_ErrExit("Should be prototyped: \"a b c * +");

        std::istringstream  iss(argv[1]);
        std::string         token;

        while (iss >> token) {
            if (token.size() > 1)
                F_ErrExit("Invalid token : " + token);
            if (std::isdigit(token[0]))
                instance.getStack().push(token[0] - '0');
            else
                instance.apply(token[0]);
        }

        if (instance.getStack().size() != 1)
            F_ErrExit("Incorrect formulation");
        std::cout << BOLD_WHITE << "RESULT = " << instance.getStack().top() << endofline;

    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (FAILURE);
    }
    return (SUCCESS);
}
