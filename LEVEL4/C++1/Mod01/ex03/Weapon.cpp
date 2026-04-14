#include "Weapon.hpp"
#include <iostream>

Weapon::Weapon(std::string s_type) {
	type = s_type;
	std::cout << type << " Has been created!\n";
}

Weapon::~Weapon() {
}

std::string	Weapon::getType(void) const {
	return (type);
}

void	Weapon::setType(std::string s_type) {
	type = s_type;
}