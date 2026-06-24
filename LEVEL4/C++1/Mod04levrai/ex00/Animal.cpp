#include <iostream>
#include "Animal.hpp"
#include "utils.hpp"

Animal::Animal(void): type("Animal") {
    std::cout << BOLD_CYAN << "Animal Name constructor called" << endofline;
}

Animal::Animal(std::string animal_type): type(animal_type) {
    std::cout << BOLD_CYAN << "Animal Name constructor called" << endofline;
}

Animal::Animal(const Animal &src) {
    std::cout << BOLD_BLUE << "Animal Copy constructor called" << endofline;
    *this = src;
}

Animal& Animal::operator= (const Animal &other) {
    std::cout << BOLD_BLUE << "Animal Copy assignment operator called" << endofline;
    if (this != &other)
        type = other.type;
    return (*this);
}

Animal::~Animal() {
    std::cout << BOLD_RED << "Animal Destructor called" << endofline;
}

std::string Animal::getType(void) const {
    return (type);
}

void    Animal::makeSound(void) const {
    std::cout << BOLD_WHITE << "Doing random animal noise." << endofline;
}