#include <iostream>
#include "DiamondTrap.hpp"
#include "FragTrap.hpp"
#include "utils.hpp"

DiamondTrap::DiamondTrap(std::string name) : ClapTrap(name + "_clap_name"), ScavTrap(name), FragTrap(name), _name(name) {
    _hp = FragTrap::_base_hp;
    _energy = ScavTrap::_base_energy;
    _atk = FragTrap::_base_atk;
    std::cout << BOLD_CYAN << "DiamondTrap Name constructor called" << endofline;
}

DiamondTrap::DiamondTrap(const DiamondTrap &src)
    : ClapTrap(src), ScavTrap(src), FragTrap(src), _name(src._name) {
    std::cout << BOLD_BLUE << "DiamondTrap Copy constructor called" << endofline;
}

DiamondTrap& DiamondTrap::operator= (const DiamondTrap &other) {
    std::cout << BOLD_BLUE << "DiamondTrap Copy assignment operator called" << endofline;
    if (this != &other) {
        ScavTrap::operator=(other);
        _name = other._name;
    }
    return (*this);
}

DiamondTrap::~DiamondTrap() {
    std::cout << BOLD_RED << "DiamondTrap Destructor called" << endofline;
}

void DiamondTrap::attack(const std::string& target) {
    ScavTrap::attack(target);
}

void DiamondTrap::whoAmI() {
    std::cout << BOLD_MAGENTA << "My name is " << _name
              << " and my ClapTrap name is " << ClapTrap::_name << endofline;
}

void    DiamondTrap::printStats(void) {
    std::cout << "Diamond trap STATS = [ HP = " << _hp << "] | [ ENERGY = " << _energy << "] | [ ATK = " << _atk << "]" << endofline;
}