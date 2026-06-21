#include "ClapTrap.hpp"
#include "utils.hpp"
#include <iostream>

ClapTrap::ClapTrap(void) : m_hp(10), m_energy(10), m_atk(0) {
	std::cout << GREEN << "ClapTrap Default constructor called" << endofline;
}

ClapTrap::ClapTrap(std::string name) : m_name(name), m_hp(10), m_energy(10), m_atk(0) {
	std::cout << GREEN << "ClapTrap Name constructor called" << endofline;
}

ClapTrap::ClapTrap(const ClapTrap &src) {
	std::cout << GREEN << "ClapTrap Copy constructor called" << endofline;
	*this = src;
}

ClapTrap &ClapTrap::operator=(const ClapTrap &other) {
	std::cout << GREEN << "ClapTrap Copy assignment operator called" << endofline;
	if (this != &other) {
		m_name = other.m_name;
		m_hp = other.m_hp;
		m_energy = other.m_energy;
		m_atk = other.m_atk;
	}
	return (*this);
}

ClapTrap::~ClapTrap() {
	std::cout << GREEN << "ClapTrap Destructor called" << endofline;
}

void ClapTrap::attack(const std::string &target) {
	if (m_energy == 0)
		std::cout << GREEN << "ClapTrap " << m_name << " Can't attack because energy's too low" << endofline;
	else if (m_hp == 0)
		std::cout << GREEN << "ClapTrap " << m_name << " Can't attack because it's dead" << endofline;
	else {
		m_energy--;
		std::cout << GREEN << "ClapTrap " << m_name << " attacks " << target << ", causing " << m_atk << " points of damage!" << endofline;
	}
}

void ClapTrap::beRepaired(unsigned int amount) {
	if (m_energy == 0)
		std::cout << GREEN << "ClapTrap " << m_name << " Can't repair because energy's too low" << endofline;
	else if (m_hp == 0)
		std::cout << GREEN << "ClapTrap " << m_name << " Can't repair because it's dead" << endofline;
	else {
		m_energy--;
		m_hp += amount;
		std::cout << GREEN << "ClapTrap " << m_name << " repaired for " << amount << " HP and is now at " << m_hp << " HP" << endofline;
	}
}

void ClapTrap::takeDamage(unsigned int amount) {
	if (m_hp == 0) {
		std::cout << GREEN << "ClapTrap " << m_name << " is already dead. Leave its corpse alone ..." << endofline;
		return;
	} else {
		std::cout << GREEN << "ClapTrap " << m_name << " took " << amount << " of damage" << endofline;
		if (amount >= m_hp) {
			m_hp = 0;
			std::cout << GREEN << "X ClapTrap " << m_name << " is dead X" << endofline;
		} else
			m_hp -= amount;
	}
}
