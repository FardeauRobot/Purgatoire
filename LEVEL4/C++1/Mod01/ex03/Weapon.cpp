#include "Weapon.hpp"
#include <iostream>

Weapon::Weapon () {};

Weapon::Weapon(std::string s_type) {
	type = s_type;
	std::cout << type << " Has been created!\n";
}

Weapon::Weapon(const Weapon &src)
{
	*this = src;
}

Weapon &Weapon::operator=(const Weapon &other)
{
	if (this != &other)
		type = other.type;
	return (*this);
}

Weapon::~Weapon() {
}

const std::string& Weapon::getType(void) const {
	return (type);
}

void Weapon::setType(const std::string &set_type) {
	type = set_type;
}