#include <iostream>
#include "FragTrap.hpp"
#include "ClapTrap.hpp"
#include "utils.hpp"

FragTrap::FragTrap(std::string name): ClapTrap(name) {
    _hp = _base_hp;
    _energy = _base_energy;
    _atk = _base_atk;
    std::cout << BOLD_CYAN << "FragTrap Name constructor called" << endofline;
}

FragTrap::FragTrap(const FragTrap &src): ClapTrap(src) {
    std::cout << BOLD_BLUE << "FragTrap Copy constructor called" << endofline;
}

FragTrap& FragTrap::operator= (const FragTrap &other) {
    std::cout << BOLD_BLUE << "FragTrap Copy assignment operator called" << endofline;
    if (this != &other)
        ClapTrap::operator=(other);
    return (*this);
}

FragTrap::~FragTrap() {
    std::cout << BOLD_RED << "FragTrap Destructor called" << endofline;
}

void    FragTrap::highFivesGuys(void) {
    std::cout << BOLD_MAGENTA << _name << " says: GIMME FIVE! :D" << endofline;
}
