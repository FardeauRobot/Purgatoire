#include <iostream>
#include "Test.hpp"
#include "utils.hpp"

Test::Test(void) {
    std::cout << BOLD_CYAN << "Test Default constructor called" << RESET << std::endl;
}

Test::Test(std::string name): m_name(name) {
    std::cout << BOLD_CYAN << "Test Name constructor called" << RESET << std::endl;
}

Test::Test(const Test &src) {
    std::cout << BOLD_BLUE << "Test Copy constructor called" << RESET << std::endl;
    *this = src;
}

Test& Test::operator= (const Test &other) {
    std::cout << BOLD_BLUE << "Test Copy assignment operator called" << RESET << std::endl;
    if (this != &other)
        m_name = other.m_name;
    return (*this);
}

Test::~Test() {
    std::cout << BOLD_RED << "Test Destructor called" << RESET << std::endl;
}
