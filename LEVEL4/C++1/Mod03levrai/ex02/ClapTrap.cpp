#include <iostream>
#include "utils.hpp"

#include "ClapTrap.hpp"

ClapTrap::ClapTrap(std::string name): _name(name), _hp(10), _energy(10), _atk(0) {
    std::cout << BOLD_CYAN << "ClapTrap Name constructor called" << endofline;
}

ClapTrap::ClapTrap(const ClapTrap &src): _name(src._name), _hp(src._hp), _energy(src._energy), _atk(src._atk) {
    std::cout << BOLD_BLUE << "ClapTrap Copy constructor called" << endofline;
}

ClapTrap& ClapTrap::operator= (const ClapTrap &other) {
    std::cout << BOLD_BLUE << "ClapTrap Copy assignment operator called" << endofline;
    if (this != &other) {
        _name = other._name;
        _hp = other._hp;
        _energy = other._energy;
        _atk = other._atk;
    }
    return (*this);
}

ClapTrap::~ClapTrap() {
    std::cout << BOLD_RED << "ClapTrap Destructor called" << endofline;
}

int     ClapTrap::getEnergy(void) {
    return (_energy);
}

void    ClapTrap::attack(const std::string& target) {
    if (_hp <= 0)
        std::cout << BOLD_RED << _name << " can't attack, he's dead" << endofline;
    else if (_energy <= 0)
        std::cout << BOLD_YELLOW << _name << " can't attack, no energy left" << endofline;
    else {
        _energy--;
        std::cout << BOLD_WHITE << "ClapTrap " << _name << " attacks " << target << ", causing " << _atk << " points of damage!" << endofline;
    }
}

void    ClapTrap::takeDamage(unsigned int amount) {
    if (_hp <= 0)
        std::cout << BOLD_RED << _name << " is already dead, leave its corpse alone" << endofline;
    else {
        std::cout << _name << " took " << BOLD_RED << amount << RESET << " of damage" << endofline;
        _hp -= amount;
        if (_hp <= 0)
            std::cout << BOLD_RED << _name << " has been taken down" << endofline;
    }
}

void    ClapTrap::beRepaired(unsigned int amount) {
    if (_hp <= 0)
        std::cout << BOLD_RED << _name << " can't repair, he's dead" << endofline;
    else if (_energy <= 0)
        std::cout << BOLD_YELLOW << _name << " can't repair, no energy left" << endofline;
    else {
        _energy--;
        _hp += amount;
        std::cout << _name << " repaired itself for " << amount << ". It's now at " << BOLD_YELLOW << _hp << " hp." << endofline;
    }
}
