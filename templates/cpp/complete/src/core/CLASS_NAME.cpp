#include <iostream>
#include "core/CLASS_NAME.hpp"
#include "utils/utils.hpp"

CLASS_NAME::CLASS_NAME(std::string name): _name(name) {
    std::cout << BOLD_CYAN << "CLASS_NAME Name constructor called" << endofline;
}

CLASS_NAME::CLASS_NAME(const CLASS_NAME &src) {
    std::cout << BOLD_BLUE << "CLASS_NAME Copy constructor called" << endofline;
    *this = src;
}

CLASS_NAME& CLASS_NAME::operator= (const CLASS_NAME &other) {
    std::cout << BOLD_BLUE << "CLASS_NAME Copy assignment operator called" << endofline;
    if (this != &other)
        _name = other._name;
    return (*this);
}

CLASS_NAME::~CLASS_NAME() {
    std::cout << BOLD_RED << "CLASS_NAME Destructor called" << endofline;
}
