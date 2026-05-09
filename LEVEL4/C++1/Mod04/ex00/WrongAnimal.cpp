#include <iostream>
#include "utils.hpp"
#include "WrongAnimal.hpp"

WrongAnimal::WrongAnimal(void) {
    std::cout << "Default constructor called" << std::endl;
}

WrongAnimal::WrongAnimal(std::string name): m_name(name) {
    std::cout << "Name constructor called" << std::endl;
}

WrongAnimal::WrongAnimal(const WrongAnimal &src) {
    std::cout << "Copy constructor called" << std::endl;
    *this = src;
}

WrongAnimal& WrongAnimal::operator= (const WrongAnimal &other) {
    std::cout << "Copy assignment operator called" << std::endl;
    if (this != &other)
        m_name = other.m_name;
    return (*this);
}

WrongAnimal::~WrongAnimal() {
    std::cout << "Destructor called" << std::endl;
}

void    WrongAnimal::wrongSound(void) {
    std::cout << BOLD_MAGENTA << "TchoooooTchooooo" << std::endl;
}