#include <iostream>
#include "Weapon.hpp"
#include "utils.hpp"

Weapon::Weapon(void) {
    std::cout << BOLD_CYAN << "Weapon Default constructor called" << RESET << std::endl;
}

Weapon::Weapon(std::string name): type(name) {
    std::cout << BOLD_CYAN << "Weapon Name constructor called" << RESET << std::endl;
}

Weapon::Weapon(const Weapon &src) {
    std::cout << BOLD_BLUE << "Weapon Copy constructor called" << RESET << std::endl;
    *this = src;
}

Weapon& Weapon::operator= (const Weapon &other) {
    std::cout << BOLD_BLUE << "Weapon Copy assignment operator called" << RESET << std::endl;
    if (this != &other)
        type = other.type;
    return (*this);
}

Weapon::~Weapon() {
    std::cout << BOLD_RED << "Weapon Destructor called" << RESET << std::endl;
}

const std::string &Weapon::getType(void) {
    return (type);
}

void    Weapon::setType(std::string change_to) {
    type = change_to;
}

void    Weapon::printType(void) {
    std::cout  << "Weapon type is: " << BOLD_CYAN << type << endofline;
}