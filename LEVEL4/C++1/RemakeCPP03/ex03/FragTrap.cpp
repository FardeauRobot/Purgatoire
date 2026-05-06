#include <iostream>
#include "utils.hpp"
#include "FragTrap.hpp"

FragTrap::FragTrap(void): ClapTrap() {
    std::cout << BOLD_GREEN <<  "FragTrap Default constructor called" << endofline;
    m_hp = 100;
    m_energy = 100;
    m_atk = 30;
}

FragTrap::FragTrap(std::string name): ClapTrap(name) {
    std::cout << BOLD_GREEN <<  "FragTrap Name constructor called" << endofline;
    m_hp = 100;
    m_energy = 100;
    m_atk = 30;
}

FragTrap::FragTrap(const FragTrap &src) {
    std::cout << BOLD_GREEN <<  "FragTrap Copy constructor called" << endofline;
    *this = src;
}

FragTrap& FragTrap::operator= (const FragTrap &other) {
    std::cout << BOLD_GREEN <<  "FragTrap Copy assignement operator called" << endofline;
    if (this != &other)
    {
        m_name = other.m_name;
        m_hp = other.m_hp;
        m_energy = other.m_energy;
        m_atk = other.m_atk;
    }
    return (*this);
}

FragTrap::~FragTrap() {
    std::cout << BOLD_MAGENTA <<  "FragTrap destructor called" << endofline;
}

void FragTrap::attack(const std::string &target) {
    if (m_energy == 0)
        std::cout << RED << "FragTrap " << m_name << " Can't attack because energy's too low" << endofline;
    else if (m_hp == 0)
        std::cout << RED << "FragTrap " << m_name << " Can't attack because it's dead" << endofline;
    else
    {
        m_energy--;
        std::cout << YELLOW << "FragTrap " << m_name << " attacks " << target << ", " << BOLD_YELLOW << "causing " << m_atk << " points of damage!" << endofline;
    }
}

void FragTrap::highFivesGuys(void) {
    std::cout << BLINK << "FragTrap " << m_name << " says : GIMME FIIIIIIVE" << endofline;
}
