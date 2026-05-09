#include <iostream>
#include "WrongCat.hpp"

WrongCat::WrongCat(void) {
    std::cout << "WrongCat Default constructor called" << std::endl;
}

WrongCat::WrongCat(std::string name): m_name(name) {
    std::cout << "WrongCat Name constructor called" << std::endl;
}

WrongCat::WrongCat(const WrongCat &src) {
    std::cout << "WrongCat Copy constructor called" << std::endl;
    *this = src;
}

WrongCat& WrongCat::operator= (const WrongCat &other) {
    std::cout << "WrongCat Copy assignment operator called" << std::endl;
    if (this != &other)
        m_name = other.m_name;
    return (*this);
}

WrongCat::~WrongCat() {
    std::cout << "WrongCat Destructor called" << std::endl;
}
