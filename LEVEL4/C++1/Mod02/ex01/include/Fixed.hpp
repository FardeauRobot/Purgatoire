#ifndef FIXED_HPP
# define FIXED_HPP

#include <cmath>

class Fixed {
    private:
        int m_raw;
        static const int m_fract_bits = 8;

    public:
        Fixed(void);
        Fixed(const int value);
        Fixed(const float value);
        Fixed(const Fixed &src);
        Fixed& operator=(const Fixed &other);
        ~Fixed();
        
        int getRawBits(void)const;
        void setRawBits(int const raw);
        float toFloat(void);
        int toInt(void);
} ;

std::ostream& operator<<(std::ostream& out, const Fixed& fixed);

#endif