#include <iostream>
#include "core/CLASS_NAME.hpp"
#include "utils/utils.hpp"

CLASS_NAME::CLASS_NAME(void) {
    std::cout << BOLD_CYAN << "CLASS_NAME Default constructor called" << RESET << std::endl;
}

CLASS_NAME::CLASS_NAME(std::string name): m_name(name) {
    std::cout << BOLD_CYAN << "CLASS_NAME Name constructor called" << RESET << std::endl;
}

CLASS_NAME::CLASS_NAME(const CLASS_NAME &src) {
    std::cout << BOLD_BLUE << "CLASS_NAME Copy constructor called" << RESET << std::endl;
    *this = src;
}

CLASS_NAME& CLASS_NAME::operator= (const CLASS_NAME &other) {
    std::cout << BOLD_BLUE << "CLASS_NAME Copy assignment operator called" << RESET << std::endl;
    if (this != &other)
        m_name = other.m_name;
    return (*this);
}

CLASS_NAME::~CLASS_NAME() {
    std::cout << BOLD_RED << "CLASS_NAME Destructor called" << RESET << std::endl;
}
