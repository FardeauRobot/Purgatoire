#include <iostream>
#include <string>
#include "ClapTrap.hpp"
#include "colors.hpp"

ClapTrap::ClapTrap(std::string name): m_name(name), m_hp(10), m_energy(10), m_atk(0) {
    std::cout << BOLD_CYAN << "Default constructor called" << RESET << std::endl;
}

ClapTrap::ClapTrap(const ClapTrap &src) {
    std::cout << BOLD_BLUE << "Copy constructor called" << RESET << std::endl;
    *this = src;
}

ClapTrap& ClapTrap::operator= (const ClapTrap &other) {
    std::cout << BOLD_BLUE << "Copy assignment constructor called" << RESET << std::endl;
    if (this != &other)
    {
        m_name = other.m_name;
        m_atk = other.m_atk;
        m_energy = other.m_energy;
        m_hp = other.m_hp;
    }
    return (*this);
}

ClapTrap::~ClapTrap() {
    std::cout   << BOLD_RED << "Destructor called" << RESET << std::endl;
}

void ClapTrap::attack(const std::string& target) {
    if (m_energy > 0)
    {
        m_energy--;
        std::cout   << "ClapTrap " << m_name << " attacks " << target
                    << ", causing " << m_atk << " points of damage!" << std::endl;
    }
    else
        std::cout   << YELLOW << m_name << " doesn't have any energy left :/" << RESET << std::endl;
}

void ClapTrap::takeDamage(unsigned int amount) {
    if (m_hp > 0)
    {
        m_hp -= amount;
        if (m_hp <= 0)
        {
            m_hp = 0;
            std::cout   << BOLD_RED << m_name << " died." << RESET << std::endl;
        }
        else
        {
            std::cout   << RED << m_name << " took " << amount << " damage(s)" << RESET << std::endl;
            std::cout   << m_name << " has " << BOLD_WHITE << m_hp << " HP left :(" << RESET << std::endl;
        }
    }
    else
        std::cout << BOLD_YELLOW << m_name << " is already dead, please stop ..." << RESET << std::endl ;
}

void ClapTrap::beRepaired(unsigned int amount) {
    if (m_energy > 0)
    {
        m_hp += amount;
        m_energy--;
        std::cout   << GREEN << m_name << " repaired for " << amount << " HP" << RESET << std::endl;
        std::cout   << m_name << " has " << BOLD_WHITE << m_hp << " HP" << RESET " left:D"<< std::endl;
    }
    else
        std::cout   << YELLOW << m_name << " doesn't have any energy left :/" << RESET << std::endl;
}