#include "Errors.hpp"
#include "colors.hpp"
#include <iostream>
#include <cstdlib>

int F_ErrMsg(const std::string& msg)
{
    std::cerr << BOLD_RED << "Error: " << RESET << msg << std::endl;
    return (FAILURE);
}

void F_WarnMsg(const std::string& msg)
{
    std::cerr << BOLD_YELLOW << "Warning: " << RESET << msg << std::endl;
}

void F_ErrExit(const std::string& msg)
{
    std::cerr << BOLD_RED << "Fatal: " << RESET << msg << std::endl;
    std::exit(FAILURE);
}
