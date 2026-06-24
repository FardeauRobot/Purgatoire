#include <iostream>

#include "Animal.hpp"
#include "Cat.hpp"
#include "utils.hpp"

Cat::Cat(void): Animal() {
    type = "Cat";
    std::cout << BOLD_CYAN << "Cat Name constructor called" << endofline;
}

Cat::Cat(const Cat &src) {
    std::cout << BOLD_BLUE << "Cat Copy constructor called" << endofline;
    *this = src;
}

Cat& Cat::operator= (const Cat &other) {
    std::cout << BOLD_BLUE << "Cat Copy assignment operator called" << endofline;
    if (this != &other)
        type = other.type;
    return (*this);
}

Cat::~Cat() {
    std::cout << BOLD_RED << "Cat Destructor called" << endofline;
}


void Cat::makeSound(void) const {
    std::cout << BOLD_YELLOW << "MEEEEEEEEEOW" << endofline;
}