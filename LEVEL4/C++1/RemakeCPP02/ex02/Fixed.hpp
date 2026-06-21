#ifndef FIXED_HPP
# define FIXED_HPP

#include <iostream>

class Fixed {
    private:
        int m_raw;
        const static int m_fract_bits = 8;

    public:
        Fixed(void);
        Fixed(int nb);
        Fixed(float nb);
        Fixed(const Fixed &src);
        Fixed& operator= (const Fixed &other);
        ~Fixed();

        int getRawBits(void) const ;
        void setRawBits(int const raw) ;
        float toFloat(void) const ;
        int toInt(void) const ;

        bool operator< (Fixed const& other) const ;
        bool operator<= (Fixed const& other) const ;
        bool operator> (Fixed const& other) const ;
        bool operator>= (Fixed const& other) const ;
        bool operator== (Fixed const& other) const ;
        bool operator!= (Fixed const& other) const ;

        Fixed    operator+ (Fixed const& other) const ;
        Fixed    operator- (Fixed const& other) const ;
        Fixed    operator* (Fixed const& other) const ;
        Fixed    operator/ (Fixed const& other) const ;

        static Fixed const& min (Fixed &lhs, Fixed &rhs) ;
        static Fixed const& min (const Fixed &lhs, const Fixed &rhs) ;
        static Fixed const& max (Fixed &lhs, Fixed &rhs) ;
        static Fixed const& max (const Fixed &lhs, const Fixed &rhs) ;
        
        // PREFIX
        Fixed& operator++() ;
        Fixed& operator--() ;

        // POSTFIX
        Fixed operator++(int) ;
        Fixed operator--(int) ;
};

std::ostream& operator<< (std::ostream& os, Fixed const& fixed);

#endif
