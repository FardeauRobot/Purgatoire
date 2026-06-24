#include <iostream>

#include "Animal.hpp"
#include "Brain.hpp"
#include "Cat.hpp"
#include "utils.hpp"

Cat::Cat(void): Animal("Cat") {
    std::cout << BOLD_CYAN << "Cat Name constructor called" << endofline;
    _brain = new Brain; 
}

Cat::Cat(const Cat &src) {
    std::cout << BOLD_BLUE << "Cat Copy constructor called" << endofline;
    _brain = NULL;
    *this = src;
}

Cat& Cat::operator= (const Cat &other) {
    std::cout << BOLD_BLUE << "Cat Copy assignment operator called" << endofline;
    if (this != &other) {
        Animal::operator=(other);
        delete(_brain);
        _brain = new Brain(*other._brain);
    }
    return (*this);
}

Cat::~Cat() {
    delete (_brain);
    std::cout << BOLD_RED << "Cat Destructor called" << endofline;
}


void Cat::makeSound(void) const {
    std::cout << BOLD_YELLOW << "MEEEEEEEEEOW" << endofline;
}

Brain* Cat::getBrain(void) {
    return (_brain);
}