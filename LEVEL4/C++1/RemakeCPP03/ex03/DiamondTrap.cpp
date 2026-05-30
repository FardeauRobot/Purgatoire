#include <iostream>
#include "DiamondTrap.hpp"
#include "ClapTrap.hpp"
#include "ScavTrap.hpp"
#include "utils.hpp"

DiamondTrap::DiamondTrap(void): ClapTrap("_clap_name") {
	std::cout << MAGENTA << "DiamondTrap Default constructor called" << RESET << std::endl;
	m_name = "";
	m_hp = FragTrap::m_hp;
	m_energy = 50;
	m_atk = FragTrap::m_atk;
}

DiamondTrap::DiamondTrap(std::string name): ClapTrap(name + "_clap_name") {
	std::cout << MAGENTA << "DiamondTrap Name constructor called" << RESET << std::endl;
	m_name = name;
	m_hp = FragTrap::m_hp;
	m_energy = 50;
	m_atk = FragTrap::m_atk;
}

DiamondTrap::DiamondTrap(const DiamondTrap &src) : ClapTrap(src), ScavTrap(src), FragTrap(src) {
	std::cout << MAGENTA << "DiamondTrap Copy constructor called" << RESET << std::endl;
	*this = src;
}

DiamondTrap& DiamondTrap::operator= (const DiamondTrap &other) {
	std::cout << MAGENTA << "DiamondTrap Copy assignment operator called" << RESET << std::endl;
	if (this != &other)
	{
		ClapTrap::operator=(other);
		m_name = other.m_name;
		m_atk = other.m_atk;
		m_hp = other.m_hp;
		m_energy = other.m_energy;
	}
	return (*this);
}

DiamondTrap::~DiamondTrap() {
	std::cout << MAGENTA << "DiamondTrap Destructor called" << RESET << std::endl;
}

void	DiamondTrap::attack(const std::string &target) {
	ScavTrap::attack(target);
}

void	DiamondTrap::whoAmI() {
	std::cout	<< BOLD_YELLOW << "This diamond trap comes from " << ClapTrap::m_name <<
				" and is named " << m_name << endofline;
}


int	DiamondTrap::getEnergy(void) const {
	return (m_energy);
}