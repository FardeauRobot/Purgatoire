#include <iostream>
#include <cmath>
#include "Fixed.hpp"
#include "utils.hpp"

Fixed::Fixed(void) : m_raw (0) {
    // std::cout << BOLD_CYAN << "Fixed Default constructor called" << RESET << std::endl;
}

Fixed::Fixed(int nb) {
    // std::cout << BOLD_MAGENTA << "Fixed Int constructor called" << RESET << std::endl;
    m_raw = nb << 8;
}

Fixed::Fixed(float nb) {
    // std::cout << BOLD_MAGENTA << "Fixed Float constructor called" << RESET << std::endl;
    m_raw = static_cast<int>(roundf(nb * (1 << m_fract_bits)));
}

Fixed::Fixed(const Fixed &src) : m_raw(src.m_raw) {
    // std::cout << BOLD_BLUE << "Fixed Copy constructor called" << RESET << std::endl;
}

Fixed& Fixed::operator= (const Fixed &other) {
    // std::cout << BOLD_BLUE << "Fixed Copy assignment operator called" << RESET << std::endl;
    if (this != &other)
        m_raw = other.getRawBits();
    return (*this);
}

Fixed::~Fixed() {
    // std::cout << BOLD_RED << "Fixed Destructor called" << RESET << std::endl;
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
    return (m_raw >> m_fract_bits);
}

float Fixed::toFloat(void) const {
    return (m_raw / static_cast<float>(1 << m_fract_bits));
}

bool    Fixed::operator< (Fixed const& other) const {
    return (this->m_raw < other.m_raw);
}

bool    Fixed::operator<= (Fixed const& other) const {
    return (this->m_raw <= other.m_raw);
}

bool    Fixed::operator> (Fixed const& other) const {
    return (this->m_raw > other.m_raw);
}

bool    Fixed::operator>= (Fixed const& other) const {
    return (this->m_raw >= other.m_raw);
}

bool    Fixed::operator== (Fixed const& other) const {
    return (this->m_raw == other.m_raw);
}

bool    Fixed::operator!= (Fixed const& other) const {
    return (this->m_raw != other.m_raw);
}

Fixed    Fixed::operator+ (Fixed const& other) const {
    Fixed   result;
    result.setRawBits(this->m_raw + other.m_raw);
    return (result);
}

Fixed    Fixed::operator- (Fixed const& other) const {
    Fixed   result;
    result.setRawBits(this->m_raw - other.m_raw);
    return (result);
}

Fixed    Fixed::operator* (Fixed const& other) const {
    Fixed   result;
    result.setRawBits((this->m_raw * other.m_raw) >> m_fract_bits);
    return (result);
}

Fixed    Fixed::operator/ (Fixed const& other) const {
    Fixed   result;
    result.setRawBits((this->m_raw << m_fract_bits) / other.m_raw);
    return (result);
}

Fixed const& Fixed::min (Fixed &lhs, Fixed &rhs) {
    return (lhs < rhs ? lhs : rhs);
}

Fixed const& Fixed::min (const Fixed &lhs, const Fixed &rhs) {
    return (lhs < rhs ? lhs : rhs);
}

Fixed const& Fixed::max (Fixed &lhs, Fixed &rhs) {
    return (lhs > rhs ? lhs : rhs);
}

Fixed const& Fixed::max (const Fixed &lhs, const Fixed &rhs) {
    return (lhs > rhs ? lhs : rhs);
}

Fixed& Fixed::operator++() {
    ++m_raw;
    return (*this);
}

Fixed& Fixed::operator--() {
    --m_raw;
    return (*this);
}

Fixed Fixed::operator++(int) {
    Fixed tmp(*this);
    ++m_raw;
    return (tmp);
}

Fixed Fixed::operator--(int) {
    Fixed tmp(*this);
    --m_raw;
    return (tmp);
}

std::ostream& operator<< (std::ostream& os, Fixed const& fixed) {
    os << fixed.toFloat();
    return (os);
}