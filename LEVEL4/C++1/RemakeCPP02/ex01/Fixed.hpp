#ifndef FIXED_HPP
# define FIXED_HPP

#include <string>

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

};

std::ostream& operator<< (std::ostream& os, Fixed const& fixed);

#endif
