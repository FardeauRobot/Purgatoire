#include <iostream>

#include "utils.hpp"
#include "ScavTrap.hpp"
#include "ClapTrap.hpp"

ScavTrap::ScavTrap(std::string name): ClapTrap(name), _protect(false) {
    _hp = 100;
    _energy = 50;
    _atk = 20;
    std::cout << BOLD_CYAN << "ScavTrap Name constructor called" << endofline;
}

ScavTrap::ScavTrap(const ScavTrap &src): ClapTrap(src), _protect(src._protect) {
    std::cout << BOLD_BLUE << "ScavTrap Copy constructor called" << endofline;
}

ScavTrap& ScavTrap::operator= (const ScavTrap &other) {
    std::cout << BOLD_BLUE << "ScavTrap Copy assignment operator called" << endofline;
    if (this != &other) {
        ClapTrap::operator=(other);
        _protect = other._protect;
    }
    return (*this);
}

ScavTrap::~ScavTrap() {
    std::cout << BOLD_RED << "ScavTrap Destructor called" << endofline;
}

void    ScavTrap::attack(const std::string& target) {
    if (_hp <= 0)
        std::cout << BOLD_RED << _name << " can't attack, he's dead" << endofline;
    else if (_energy <= 0)
        std::cout << BOLD_YELLOW << _name << " can't attack, no energy left" << endofline;
    else {
        _energy--;
        std::cout << BOLD_WHITE << "ScavTrap " << _name << " attacks " << target << ", causing " << _atk << " points of damage!" << endofline;
    }
}

void ScavTrap::guardGate() {
    if (_protect == false) {
        _protect = true;
        std::cout << BOLD_WHITE << _name << " is now guarding the gate." << endofline;
    }
    else
        std::cout << BOLD_WHITE << _name << " is already guarding the gate" << endofline;
}
