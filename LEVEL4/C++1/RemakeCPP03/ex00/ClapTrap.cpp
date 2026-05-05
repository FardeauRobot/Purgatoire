#include <iostream>
#include "ClapTrap.hpp"
#include "utils.hpp"

ClapTrap::ClapTrap(void): m_hp(10), m_energy(10), m_atk(0) {
    std::cout << BOLD_CYAN << "Default constructor called" << RESET << std::endl;
}

ClapTrap::ClapTrap(std::string name): m_name(name), m_hp(10), m_energy(10), m_atk(0) {
    std::cout << BOLD_CYAN << "Name constructor called" << RESET << std::endl;
}

ClapTrap::ClapTrap(const ClapTrap &src) {
    std::cout << BOLD_BLUE << "Copy constructor called" << RESET << std::endl;
    *this = src;
}

ClapTrap& ClapTrap::operator= (const ClapTrap &other) {
    std::cout << BOLD_BLUE << "Copy assignment operator called" << RESET << std::endl;
    if (this != &other)
    {
        m_name = other.m_name;
        m_hp = other.m_hp;
        m_energy = other.m_energy;
        m_atk = other.m_atk;
    }
    return (*this);
}

ClapTrap::~ClapTrap() {
    std::cout << BOLD_RED << "Destructor called" << RESET << std::endl;
}

void attack(const std::string &target) {

}