#include <iostream>
#include "WrongAnimal.hpp"
#include "utils.hpp"

WrongAnimal::WrongAnimal(void) : type("WrongAnimal") {
    std::cout << BOLD_CYAN << "WrongAnimal Default constructor called" << endofline;
}

WrongAnimal::WrongAnimal(std::string animal_type): type(animal_type) {
    std::cout << BOLD_CYAN << "WrongAnimal Name constructor called" << endofline;
}

WrongAnimal::WrongAnimal(const WrongAnimal &src) {
    std::cout << BOLD_BLUE << "WrongAnimal Copy constructor called" << endofline;
    *this = src;
}

WrongAnimal& WrongAnimal::operator= (const WrongAnimal &other) {
    std::cout << BOLD_BLUE << "WrongAnimal Copy assignment operator called" << endofline;
    if (this != &other)
        type = other.type;
    return (*this);
}

WrongAnimal::~WrongAnimal() {
    std::cout << BOLD_RED << "WrongAnimal Destructor called" << endofline;
}

void WrongAnimal::makeSound(void) const {
    std::cout << BOLD_WHITE << type << " is doing random not animal noise" << endofline;
}

std::string WrongAnimal::getType(void) const {
    return (type);
}