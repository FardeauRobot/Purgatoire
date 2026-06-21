#include <iostream>
#include "ScavTrap.hpp"
#include "utils.hpp"

ScavTrap::ScavTrap(void) : ClapTrap() {
    std::cout << CYAN << "ScavTrap Default constructor called" << endofline;
    m_hp = 100;
    m_energy = 50;
    m_atk = 20;
    m_guarding = false;
}

ScavTrap::ScavTrap(std::string name): ClapTrap(name) {
    std::cout << CYAN << "ScavTrap Name constructor called" << endofline;
    m_hp = 100;
    m_energy = 50;
    m_atk = 20;
    m_guarding = false;
}

ScavTrap::ScavTrap(const ScavTrap &src) : ClapTrap(src) {
    std::cout << CYAN << "ScavTrap Copy constructor called" << endofline;
    *this = src;
}

ScavTrap& ScavTrap::operator= (const ScavTrap &other) {
    std::cout << CYAN << "ScavTrap Copy assignment operator called" << endofline;
    if (this != &other)
    {
        m_name = other.m_name;
        m_hp = other.m_hp;
        m_energy = other.m_energy;
        m_atk = other.m_atk;
        m_guarding = other.m_guarding;
    }
    return (*this);
}

ScavTrap::~ScavTrap() {
    std::cout << CYAN << "ScavTrap Destructor called" << endofline;
}

void    ScavTrap::guardGate(void) {
    if (m_guarding == false)
    {
        m_guarding = true;
        std::cout << CYAN << "ScavTrap " << m_name << " is now guarding the Gate" << endofline;
    }
    else
        std::cout << CYAN << "ScavTrap " << m_name << " is already guarding the Gate" << endofline;
}

void ScavTrap::attack(const std::string &target) {
    if (m_energy == 0)
        std::cout << CYAN << "ScavTrap " << m_name << " Can't attack because energy's too low" << endofline;
    else if (m_hp == 0)
        std::cout << CYAN << "ScavTrap " << m_name << " Can't attack because it's dead" << endofline;
    else
    {
        m_energy--;
        std::cout << CYAN << "ScavTrap " << m_name << " attacks " << target << ", causing " << m_atk << " points of damage!" << endofline;
    }
}

void ScavTrap::beRepaired(unsigned int amount) {
    if (m_energy == 0)
        std::cout << CYAN << "ScavTrap " << m_name << " Can't repair because energy's too low" << endofline;
    else
    {
        m_energy--;
        m_hp += amount;
        std::cout << CYAN << "ScavTrap " << m_name << " repaired for " << amount << " HP and is now at " << m_hp << " HP" << endofline;
    }
}

void ScavTrap::takeDamage(unsigned int amount) {
    if (m_hp == 0)
    {
        std::cout << CYAN << "ScavTrap " << m_name << " is already dead. Leave its corpse alone ..." << endofline;
        return;
    }
    std::cout << CYAN << "ScavTrap " << m_name << " took " << amount << " of damage" << endofline;
    if (amount >= m_hp)
    {
        m_hp = 0;
        std::cout << CYAN << "X ScavTrap " << m_name << " is dead X" << endofline;
    }
    else
        m_hp -= amount;
}
