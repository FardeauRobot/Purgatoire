#include "Fixed.hpp"

Fixed::Fixed(void)
{
}

Fixed::Fixed(const int value)
{
    (void)value;
}

Fixed::Fixed(const float value)
{
    (void)value;
}

Fixed::Fixed(const Fixed &src)
{
    (void)src;
}

Fixed::~Fixed(void)
{
}

Fixed &Fixed::operator=(const Fixed &other)
{
    (void)other;
    return (*this);
}

bool Fixed::operator>(const Fixed &other) const
{
    (void)other;
    return (false);
}

bool Fixed::operator<(const Fixed &other) const
{
    (void)other;
    return (false);
}

bool Fixed::operator>=(const Fixed &other) const
{
    (void)other;
    return (false);
}

bool Fixed::operator<=(const Fixed &other) const
{
    (void)other;
    return (false);
}

bool Fixed::operator==(const Fixed &other) const
{
    (void)other;
    return (false);
}

bool Fixed::operator!=(const Fixed &other) const
{
    (void)other;
    return (false);
}

Fixed Fixed::operator+(const Fixed &other) const
{
    (void)other;
    return (Fixed());
}

Fixed Fixed::operator-(const Fixed &other) const
{
    (void)other;
    return (Fixed());
}

Fixed Fixed::operator*(const Fixed &other) const
{
    (void)other;
    return (Fixed());
}

Fixed Fixed::operator/(const Fixed &other) const
{
    (void)other;
    return (Fixed());
}

Fixed &Fixed::operator++(void)
{
    return (*this);
}

Fixed Fixed::operator++(int)
{
    return (Fixed());
}

Fixed &Fixed::operator--(void)
{
    return (*this);
}

Fixed Fixed::operator--(int)
{
    return (Fixed());
}

int Fixed::getRawBits(void) const
{
    return (m_raw);
}

void Fixed::setRawBits(int const raw)
{
    m_raw = raw;
}

float Fixed::toFloat(void) const
{
    return (0.0f);
}

int Fixed::toInt(void) const
{
    return (0);
}

Fixed &Fixed::min(Fixed &a, Fixed &b)
{
    (void)b;
    return (a);
}

const Fixed &Fixed::min(const Fixed &a, const Fixed &b)
{
    (void)b;
    return (a);
}

Fixed &Fixed::max(Fixed &a, Fixed &b)
{
    (void)b;
    return (a);
}

const Fixed &Fixed::max(const Fixed &a, const Fixed &b)
{
    (void)b;
    return (a);
}

std::ostream &operator<<(std::ostream &out, const Fixed &fixed)
{
    (void)fixed;
    return (out);
}
