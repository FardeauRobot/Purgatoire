#include <iostream>
#include "Fixed.hpp"
#include "utils.hpp"

Fixed::Fixed(void) : m_raw (0) {
    std::cout << BOLD_CYAN << "Fixed Default constructor called" << RESET << std::endl;
}

Fixed::Fixed(const Fixed &src) {
    std::cout << BOLD_BLUE << "Fixed Copy constructor called" << RESET << std::endl;
    *this = src;
}

Fixed& Fixed::operator= (const Fixed &other) {
    std::cout << BOLD_BLUE << "Fixed Copy assignment operator called" << RESET << std::endl;
    if (this != &other)
        m_raw = other.getRawBits();
    return (*this);
}

Fixed::~Fixed() {
    std::cout << BOLD_RED << "Fixed Destructor called" << RESET << std::endl;
}

int Fixed::getRawBits(void) const {
    std::cout << BOLD_GREEN << "getRawBits member function called" << RESET << std::endl;
    return (m_raw);
}
void    Fixed::setRawBits(int const raw) {
    std::cout << BOLD_YELLOW << "setRawBits member function called" << RESET << std::endl;
    m_raw = raw;
}