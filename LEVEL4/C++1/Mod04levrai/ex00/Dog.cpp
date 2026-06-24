#include <iostream>

#include "Animal.hpp"
#include "Dog.hpp"
#include "utils.hpp"

Dog::Dog(void): Animal() {
    type = "Dog";
    std::cout << BOLD_CYAN << "Dog Name constructor called" << endofline;
}

Dog::Dog(const Dog &src) {
    std::cout << BOLD_BLUE << "Dog Copy constructor called" << endofline;
    *this = src;
}

Dog& Dog::operator= (const Dog &other) {
    std::cout << BOLD_BLUE << "Dog Copy assignment operator called" << endofline;
    if (this != &other)
        type = other.type;
    return (*this);
}

Dog::~Dog() {
    std::cout << BOLD_RED << "Dog Destructor called" << endofline;
}


void Dog::makeSound(void) const {
    std::cout << BOLD_YELLOW << "WOOOOOOOOOOOOOOOOOOOOOF" << endofline;
}