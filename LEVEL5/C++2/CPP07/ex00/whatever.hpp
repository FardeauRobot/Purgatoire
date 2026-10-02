#ifndef WHATEVER_HPP
# define WHATEVER_HPP

#include <iostream>

template<typename T> 
void swap(T &a, T &b) {
    
    T change = a;
    a = b;
    b = change;

}

template<typename T> 
T min(T a, T b) {
    return (a < b ? a : b);
}

template<typename T> 
T max(T a, T b) {
    return (a > b ? a : b);
}

class whatever {
    public:
        whatever();
        whatever(const whatever &src);
        whatever& operator= (const whatever &other);
        ~whatever();
};

#endif
