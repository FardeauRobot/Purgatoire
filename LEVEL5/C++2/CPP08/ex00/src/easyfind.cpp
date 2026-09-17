#include <iostream>
#include "easyfind.hpp"
#include "utils.hpp"

easyfind::easyfind(std::string name): _name(name) {
    std::cout << BOLD_CYAN << "easyfind Name constructor called" << endofline;
}

easyfind::easyfind(const easyfind &src) {
    std::cout << BOLD_BLUE << "easyfind Copy constructor called" << endofline;
    *this = src;
}

easyfind& easyfind::operator= (const easyfind &other) {
    std::cout << BOLD_BLUE << "easyfind Copy assignment operator called" << endofline;
    if (this != &other)
        _name = other._name;
    return (*this);
}

easyfind::~easyfind() {
    std::cout << BOLD_RED << "easyfind Destructor called" << endofline;
}
