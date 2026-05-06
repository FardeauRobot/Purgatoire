#include <iostream>
#include "ClapTrap.hpp"
#include "utils.hpp"

ClapTrap::ClapTrap(void): m_hp(10), m_energy(10), m_atk(0) {
    std::cout << BOLD_CYAN << "ClapTrap Default constructor called" << RESET << std::endl;
}

ClapTrap::ClapTrap(std::string name): m_name(name), m_hp(10), m_energy(10), m_atk(0) {
    std::cout << BOLD_CYAN << "ClapTrap Name constructor called" << RESET << std::endl;
}

ClapTrap::ClapTrap(const ClapTrap &src) {
    std::cout << BOLD_BLUE << "ClapTrap Copy constructor called" << RESET << std::endl;
    *this = src;
}

ClapTrap& ClapTrap::operator= (const ClapTrap &other) {
    std::cout << BOLD_BLUE << "ClapTrap Copy assignment operator called" << RESET << std::endl;
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
    std::cout << BOLD_RED << "ClapTrap Destructor called" << RESET << std::endl;
}

void ClapTrap::attack(const std::string &target) {
    if (m_energy == 0)
        std::cout << RED << "ClapTrap " << m_name << " Can't attack because energy's too low" << endofline;
    else if (m_hp == 0)
        std::cout << RED << "ClapTrap " << m_name << " Can't attack because it's dead" << endofline;
    else
    {
        m_energy--;
        std::cout << YELLOW << "ClapTrap " << m_name << " attacks " << target << ", " << BOLD_YELLOW << "causing " << m_atk << " points of damage!" << endofline;
    }
}

void ClapTrap::beRepaired(unsigned int amount) {
    if (m_energy == 0)
        std::cout << RED << "ClapTrap " << m_name << "Can't repair because energy's too low" << endofline;
    else
    {
        m_energy--;
        m_hp += amount;
        std::cout << "ClapTrap " << m_name << " repaired for " << BOLD_GREEN << amount << RESET << " HP and is now at " << BOLD_GREEN << m_hp <<  " HP" << endofline;
    }
}

void ClapTrap::takeDamage(unsigned int amount) {
    if (m_hp == 0)
    {
        std::cout << BOLD_YELLOW << "ClapTrap " << m_name << " is already dead. Leave its corpse alone ..." << endofline;
        return;
    }
    std::cout << "ClapTrap " << m_name << " took " << BOLD_RED << amount << " of damage" << endofline;
    if (amount >= m_hp)
    {

        m_hp = 0;
        std::cout << BOLD_RED << "X ClapTrap " << m_name << " is dead X" << endofline;
    }
    else
        m_hp -= amount;
}