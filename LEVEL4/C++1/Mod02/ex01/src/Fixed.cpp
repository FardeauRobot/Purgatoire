#include "Fixed.hpp"
#include "colors.hpp"
#include <iostream>
#include <cmath>

Fixed::Fixed() {
    std::cout << BOLD_GREEN << "Default constructor called" << std::endl;
    m_raw = 0;
}

Fixed::Fixed(const int value)
{
    std::cout << BOLD_GREEN << "Int constructor called" << std::endl;
    m_raw = value << m_fract_bits;
    std::cout << BOLD_WHITE << "RAW VALUE =" << m_raw << std::endl;
}

Fixed::Fixed(const float value);
{
    std::cout << BOLD_GREEN << "float constructor called" << std::endl;
    m_raw = std::roundf(value * (1 << m_fract_bits));
    std::cout << BOLD_WHITE << "RAW VALUE =" << m_raw << std::endl;
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

float toFloat(void) {
    return (m_raw / (float)(1 <<8));
}

int toInt(void) {
    return (m_raw << 8);
}