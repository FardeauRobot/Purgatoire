#include <iostream>
#include "utils.hpp"
#include "Cat.hpp"

Cat::Cat(void) : Animal() {
    std::cout << BOLD_CYAN << "Cat Default constructor called" << endofline;
    type = "Cat";
}

Cat::Cat(const Cat &src) {
    std::cout << BOLD_CYAN << "Cat Copy constructor called" << endofline;
    *this = src;
}

Cat& Cat::operator= (const Cat &other) {
    std::cout << BOLD_CYAN << "Cat Copy assignment operator called" << endofline;
    if (this != &other)
        type = other.type;
    return (*this);
}

Cat::~Cat() {
    std::cout << BOLD_CYAN << "Cat Destructor called" << endofline;
}

void Cat::makeSound(void) const {
    std::cout << "The " << type << " says "<< BOLD_YELLOW << "MEOOOOW" << endofline;
}