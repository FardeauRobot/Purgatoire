#include <iostream>
#include "ClapTrap.hpp"
#include "colors.hpp"

ClapTrap::ClapTrap(std::string name) {
    std::cout << BOLD_GREEN << "Default constructor called" << std::endl;
    m_name = name;
}

ClapTrap::ClapTrap(const ClapTrap &src) {
    std::cout << BOLD_GREEN << "Copy constructor called" << std::endl;
    *this = src;
}

ClapTrap& ClapTrap::operator= (const ClapTrap &other) {
    std::cout << BOLD_GREEN << "Copy assignment constructor called" << std::endl;
    if (this != &other)
        m_name = other.m_name;
    return (*this);
}

ClapTrap::~ClapTrap() {}

void ClapTrap::attack(const std::string& target) {
    std::cout   << "ClapTrap " << m_name << " attacks " << target
                << ", causing " << m_atk << " points of damage!" << std::endl;
}

void ClapTrap::takeDamage(unsigned int amount) {
    m_hp - amount;
    std::cout   << BOLD_RED << m_name << " took " << amount << " of damage" << RESET << std::endl;
    std::cout   << m_name << " has " << BOLD_WHITE << m_hp << " HP left :(" << RESET << std::endl;
}