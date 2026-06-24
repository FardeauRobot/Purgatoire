#include <iostream>

#include "Animal.hpp"
#include "Dog.hpp"
#include "utils.hpp"

Dog::Dog(void): Animal() {
    type = "Dog";
    _brain = new Brain;
    std::cout << BOLD_CYAN << "Dog Name constructor called" << endofline;
}

Dog::Dog(const Dog &src) {
    std::cout << BOLD_BLUE << "Dog Copy constructor called" << endofline;
    _brain = NULL;
    *this = src;
}

Dog& Dog::operator= (const Dog &other) {
    std::cout << BOLD_BLUE << "Dog Copy assignment operator called" << endofline;
    if (this != &other) {
        Animal::operator=(other);
        delete(_brain);
        _brain = new Brain(*other._brain);
    }
    return (*this);
}

Dog::~Dog() {
    delete (_brain);
    std::cout << BOLD_RED << "Dog Destructor called" << endofline;
}


void Dog::makeSound(void) const {
    std::cout << BOLD_YELLOW << "WOOOOOOOOOOOOOOOOOOOOOF" << endofline;
}

Brain* Dog::getBrain(void) {
    return (_brain);
}