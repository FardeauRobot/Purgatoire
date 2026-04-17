#include <iostream>
#include "Zombie.hpp"
#include "colors.hpp"

Zombie::~Zombie() {
	std::cout << BOLD_RED << m_name << " has been destroyed.\n" << RESET;
}

Zombie::Zombie(void) {
	m_name = "NO NAME";
	std::cout << BOLD_CYAN << m_name << " has been created.\n" << RESET;
}
Zombie::Zombie(std::string name) {
	m_name = name;
	std::cout << BOLD_CYAN << m_name << " has been created.\n" << RESET;
}

Zombie::Zombie(const Zombie &src) {
	*this = src;
}

Zombie &Zombie::operator=(const Zombie &other) {
	if (this != &other)
		m_name = other.m_name;
	return (*this);
}

void	Zombie::F_SetName(std::string name) {
	m_name = name;
}

std::string	Zombie::F_GetName(void) {
	return (m_name);
}

void Zombie::announce(void) {
	std::cout << BOLD << m_name << RESET << ITALIC << ": BraiiiiiiinnnzzzZ...\n";
}
