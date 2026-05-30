#include <iostream>
#include <cmath>
#include "Fixed.hpp"
#include "utils.hpp"

Fixed::Fixed(void) : m_raw (0) {
    std::cout << BOLD_CYAN << "Fixed Default constructor called" << RESET << std::endl;
}

Fixed::Fixed(int nb) {
    std::cout << BOLD_MAGENTA << "Fixed Int constructor called" << RESET << std::endl;
    m_raw = nb << 8;
}

Fixed::Fixed(float nb) {
    std::cout << BOLD_MAGENTA << "Fixed Float constructor called" << RESET << std::endl;
    m_raw = roundf(nb * 256);
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
    // std::cout << BOLD_GREEN << "getRawBits member function called" << RESET << std::endl;
    return (m_raw);
}

void    Fixed::setRawBits(int const raw) {
    // std::cout << BOLD_YELLOW << "setRawBits member function called" << RESET << std::endl;
    m_raw = raw;
}

int Fixed::toInt(void) const {
    return (m_raw >> 8);
}

float Fixed::toFloat(void) const {
    return (m_raw / 256.0f);
}

std::ostream& operator<< (std::ostream& os, Fixed const& fixed) {
    os << fixed.toFloat();
    return (os);
}