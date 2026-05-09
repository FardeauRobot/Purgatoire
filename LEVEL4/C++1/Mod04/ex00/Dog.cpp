#include <iostream>
#include "utils.hpp"
#include "Dog.hpp"

Dog::Dog(void) : Animal() {
    std::cout << BOLD_BLUE << "Dog Default constructor called" << endofline;
    type = "Dog";
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
    std::cout << BOLD_BLUE << "Dog Destructor called" << endofline;
}

void Dog::makeSound(void) const {
    std::cout << "The " << type << " says " << BOLD_YELLOW << "WOOOOOOF" << endofline;
}