#include <iostream>
#include "core/Zombie.hpp"
#include "utils/utils.hpp"

Zombie::Zombie(void) {
    std::cout << BOLD_CYAN << "Zombie Default constructor called" << RESET << std::endl;
}

Zombie::Zombie(std::string name): m_name(name) {
    std::cout << BOLD_CYAN << "Zombie Name constructor called" << RESET << std::endl;
}

Zombie::Zombie(const Zombie &src) {
    std::cout << BOLD_BLUE << "Zombie Copy constructor called" << RESET << std::endl;
    *this = src;
}

Zombie& Zombie::operator= (const Zombie &other) {
    std::cout << BOLD_BLUE << "Zombie Copy assignment operator called" << RESET << std::endl;
    if (this != &other)
        m_name = other.m_name;
    return (*this);
}

Zombie::~Zombie() {
    std::cout << BOLD_RED << "Zombie Destructor called" << RESET << std::endl;
}
