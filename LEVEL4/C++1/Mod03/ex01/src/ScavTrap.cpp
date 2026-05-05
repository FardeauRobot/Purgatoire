#include <iostream>
#include "colors.hpp"
#include "ScavTrap.hpp"

ScavTrap::ScavTrap(std::string name) : ClapTrap(name){
    m_hp = 100;
    m_energy = 50;
    m_atk = 20;
    m_gatekeep = false;
    std::cout   << BOLD_CYAN << "ScavTrap default constructor called" << RESET <<std::endl;
}

ScavTrap::ScavTrap(const ScavTrap &src) : ClapTrap(src), m_gatekeep(src.m_gatekeep) {
    std::cout   << BOLD_BLUE << "ScavTrap copy constructor called" << RESET <<std::endl;
}

ScavTrap& ScavTrap::operator=(const ScavTrap &src) {
    if (this != &src)
    {
        ClapTrap::operator=(src);
        m_gatekeep = src.m_gatekeep;
    }
    std::cout   << BOLD_BLUE << "ScavTrap copy assignment operator called" << RESET <<std::endl;
    return (*this);
}

ScavTrap::~ScavTrap() {
    std::cout   << BOLD_RED << "ScavTrap destructor called" << RESET << std::endl;
}

void ScavTrap::guardGate() {
    if (m_gatekeep == false)
    {
        m_gatekeep = true;
        std::cout   << BOLD_GREEN << m_name << " is now keeping the gate" << RESET << std::endl;;
    }
    else
        std::cout   << BOLD_YELLOW << m_name << " is already keeping the gate" << RESET << std::endl;;
}
