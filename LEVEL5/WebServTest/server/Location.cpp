#include <iostream>
#include "Location.hpp"

Location::Location(void) {
    std::cout << "Default constructor called" << std::endl;
}

Location::Location(std::string name): m_name(name) {
    std::cout << "Name constructor called" << std::endl;
}

Location::Location(const Location &src) {
    std::cout << "Copy constructor called" << std::endl;
    *this = src;
}

Location& Location::operator= (const Location &other) {
    std::cout << "Copy assignment operator called" << std::endl;
    if (this != &other)
        m_name = other.m_name;
    return (*this);
}

Location::~Location() {
    std::cout << "Destructor called" << std::endl;
}
