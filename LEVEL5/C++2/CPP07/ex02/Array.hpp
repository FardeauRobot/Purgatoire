#ifndef ARRAY_HPP
# define ARRAY_HPP

#include <iostream>
#include <stdexcept>

template <typename T>
class Array {

        T* _data;
        size_t _size;

    public:
        Array();
        Array(size_t n);
        Array(const Array& other);
        ~Array();

        Array&      operator=(const Array& other);
        T&          operator[](size_t i);
        const T&    operator[](size_t i) const;
        size_t      size() const;

};

#include "Array.tpp"
#endif
