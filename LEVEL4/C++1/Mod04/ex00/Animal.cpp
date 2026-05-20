#include <iostream>
#include "utils.hpp"
#include "Animal.hpp"

Animal::Animal(void) {
	std::cout << BOLD_GREEN << "Animal Default constructor called" << RESET << std::endl;
	type = "Animal";
}

Animal::Animal(std::string type): type(type) {
	std::cout << BOLD_GREEN << "Animal Name constructor called" << RESET << std::endl;
}

Animal::Animal(const Animal &src) {
	std::cout << BOLD_BLUE << "Animal Copy constructor called" << RESET << std::endl;
	*this = src;
}

Animal& Animal::operator= (const Animal &other) {
	std::cout << BOLD_BLUE << "Animal Copy assignment operator called" << RESET << std::endl;
	if (this != &other)
		type = other.type;
	return (*this);
}

Animal::~Animal() {
	std::cout << BOLD_RED << "Animal Destructor called" << RESET << std::endl;
}

std::string Animal::getType(void) const {
	return (type);
}

void Animal::makeSound(void) const {
	std::cout << "The " << type << " says " << "Random animal noise" << std::endl;
}
