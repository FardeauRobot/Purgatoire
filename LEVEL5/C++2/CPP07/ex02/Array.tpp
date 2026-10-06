#ifndef ARRAY_TPP
# define ARRAY_TPP

#include "Array.hpp"

#define TPL template<typename T>

TPL
Array<T>::Array() : _data(NULL), _size(0) {}

TPL
Array<T>::Array(size_t n) : _data(new T[n]()), _size(n) {}

TPL
Array<T>::Array(const Array& other) : _data(new T[other._size]()), _size(other._size) {
    for (size_t i = 0; i < _size; i++)
        _data[i] = other._data[i];
}

TPL
Array<T>::~Array() {
    delete[] _data;
}

TPL
Array<T>& Array<T>::operator=(const Array& other) {
    if (this == &other)
        return (*this);

    T* fresh = new T[other._size]();
    for (size_t i = 0; i < other._size; i++)
        fresh[i] = other._data[i];

    delete[] _data;
    _data = fresh;
    _size = other._size;
    return (*this);
}

TPL
T& Array<T>::operator[](size_t i) {
    if (i >= _size)
        throw std::out_of_range("Array: index out of bounds");
    return (_data[i]);
}

TPL
const T& Array<T>::operator[](size_t i) const {
    if (i >= _size)
        throw std::out_of_range("Array: index out of bounds");
    return (_data[i]);
}

TPL
size_t Array<T>::size() const {
    return (_size);
}

#endif
