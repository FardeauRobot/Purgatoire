#include <iostream>
#include "CLASS_NAME.hpp"
#include "utils.hpp"

CLASS_NAME::CLASS_NAME(void) {
    std::cout << BOLD_CYAN << "Default constructor called" << RESET << std::endl;
}

CLASS_NAME::CLASS_NAME(std::string name): m_name(name) {
    std::cout << BOLD_CYAN << "Name constructor called" << RESET << std::endl;
}

CLASS_NAME::CLASS_NAME(const CLASS_NAME &src) {
    std::cout << BOLD_BLUE << "Copy constructor called" << RESET << std::endl;
    *this = src;
}

CLASS_NAME& CLASS_NAME::operator= (const CLASS_NAME &other) {
    std::cout << BOLD_BLUE << "Copy assignment operator called" << RESET << std::endl;
    if (this != &other)
        m_name = other.m_name;
    return (*this);
}

CLASS_NAME::~CLASS_NAME() {
    std::cout << BOLD_RED << "Destructor called" << RESET << std::endl;
}
