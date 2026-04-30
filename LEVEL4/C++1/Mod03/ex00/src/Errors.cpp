#include "Errors.hpp"
#include "colors.hpp"
#include <iostream>
#include <stdexcept>

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
    throw std::runtime_error(std::string(BOLD_RED) + "Fatal: " + RESET + msg);
}
