#include <iostream>
#include "Zombie.hpp"
#include "utils.hpp"

Zombie::Zombie(void) {
    std::cout << BOLD_CYAN << "Zombie Default constructor called" << RESET << std::endl;
}

Zombie::Zombie(std::string name): m_name(name) {
    std::cout << BOLD_CYAN << m_name << " Zombie Name constructor called" << RESET << std::endl;
}

Zombie::Zombie(const Zombie &src): m_name(src.m_name) {
    std::cout << BOLD_BLUE << m_name << " Zombie Copy constructor called" << RESET << std::endl;
    *this = src;
}

Zombie& Zombie::operator= (const Zombie &other) {
    std::cout << BOLD_BLUE << m_name << " Zombie Copy assignment operator called" << RESET << std::endl;
    if (this != &other)
        m_name = other.m_name;
    return (*this);
}

Zombie::~Zombie() {
    std::cout << BOLD_RED << m_name <<  " Zombie Destructor called" << RESET << std::endl;
}

void    Zombie::F_SetName(std::string name) {
    m_name = name;
}

std::string Zombie::F_GetName(void) const {
    return (m_name)
}

void    Zombie::announce(void) {
    std::cout << BOLD_GREEN << m_name << RESET << ": BraiiiiiiinnnzzzZ..." << endofline;
}