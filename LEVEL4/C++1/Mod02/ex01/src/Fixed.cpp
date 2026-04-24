#include "Fixed.hpp"
#include "colors.hpp"
#include <iostream>

Fixed::Fixed() {
    std::cout << BOLD_GREEN << "Default constructor called" << std::endl;
    m_raw = 0;
}

Fixed::Fixed(const Fixed &src) {
    std::cout << BOLD_GREEN << "Copy constructor called" << std::endl;
    *this=src;
}

Fixed &Fixed::operator=(const Fixed &other)
{
    std::cout << BOLD_YELLOW << "Copy assignment operator called" << std::endl;
    if (this != &other)
        m_raw = other.getRawBits();
    return (*this);
}

Fixed::~Fixed() {
    std::cout << BOLD_RED << "Destructor called" << std::endl;
}

int     Fixed::getRawBits(void) const {
    std::cout << BOLD_CYAN << "getRawBits member function called" << std::endl;
    return (m_raw);
}

void    Fixed::setRawBits(int const raw) {
    m_raw = raw;
}