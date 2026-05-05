#ifndef FIXED_HPP
# define FIXED_HPP

class Fixed {
    private:
        int m_raw;
        static const int m_fract_bits = 8;

    public:
        Fixed(void);
        Fixed(const Fixed &src);
        Fixed& operator=(const Fixed &other);
        ~Fixed();
        
        int getRawBits(void)const;
        void setRawBits(int const raw);

} ;

#endif