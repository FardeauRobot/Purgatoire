#include <iostream>
#include "whatever.hpp"
#include "utils.hpp"

whatever::whatever(std::string name): _name(name) {
    std::cout << BOLD_CYAN << "whatever Name constructor called" << endofline;
}

whatever::whatever(const whatever &src) {
    std::cout << BOLD_BLUE << "whatever Copy constructor called" << endofline;
    *this = src;
}

whatever& whatever::operator= (const whatever &other) {
    std::cout << BOLD_BLUE << "whatever Copy assignment operator called" << endofline;
    if (this != &other)
        _name = other._name;
    return (*this);
}

whatever::~whatever() {
    std::cout << BOLD_RED << "whatever Destructor called" << endofline;
}
