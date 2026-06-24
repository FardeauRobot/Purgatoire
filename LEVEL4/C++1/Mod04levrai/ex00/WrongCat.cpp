#include <iostream>

#include "WrongAnimal.hpp"
#include "WrongCat.hpp"
#include "utils.hpp"

WrongCat::WrongCat(void): WrongAnimal("WrongCat") {
    std::cout << BOLD_CYAN << "WrongCat Name constructor called" << endofline;
}

WrongCat::WrongCat(std::string animal_type): WrongAnimal(animal_type) {
    type = "WrongCat";
    std::cout << BOLD_CYAN << "WrongCat Name constructor called" << endofline;
}

WrongCat::WrongCat(const WrongCat &src) {
    std::cout << BOLD_BLUE << "WrongCat Copy constructor called" << endofline;
    *this = src;
}

WrongCat& WrongCat::operator= (const WrongCat &other) {
    std::cout << BOLD_BLUE << "WrongCat Copy assignment operator called" << endofline;
    if (this != &other)
        type = other.type;
    return (*this);
}

WrongCat::~WrongCat() {
    std::cout << BOLD_RED << "WrongCat Destructor called" << endofline;
}


void WrongCat::makeSound(void) const {
    std::cout << BOLD_YELLOW << "MEEEEEEEEEOW" << endofline;
}