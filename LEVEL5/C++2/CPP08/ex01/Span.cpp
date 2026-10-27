#include <algorithm>
#include <climits>
#include <iostream>
#include "Span.hpp"
#include "utils.hpp"

Span::Span() : _max_capacity(0) {
}

Span::Span(unsigned int capacity) : _max_capacity(capacity) {
    _storage.reserve(_max_capacity);
}

Span::Span(const Span &src) : _max_capacity(src._max_capacity), _storage(src._storage) {
}

Span& Span::operator= (const Span &other) {
    if (this != &other) {
        _max_capacity = other._max_capacity;
        _storage = other._storage;
    }
    return (*this);
}

Span::~Span() {
}

const char *Span::OutOfBoundException::what() const throw () {
    return ("No more slots available in the Span.");
}

const char *Span::TooFewElemException::what() const throw () {
    return ("Too few elements in the Span");
}

const char *Span::SpanOverflowException::what() const throw () {
    return ("Span too large to fit in an int");
}

void Span:: addNumber(int new_nbr) {
    if (_storage.size() >= _max_capacity )
        throw OutOfBoundException();
    _storage.push_back(new_nbr);
}

void    Span::printVector() const {
    for (size_t it = 0; it < _storage.size(); it++)
        std::cout << BOLD_WHITE << "NB = " << BOLD_YELLOW << _storage[it] << endofline;

}

size_t  Span::size() const {
    return (_storage.size());
}

int     Span::shortestSpan() const {
    if (_storage.size() < 2)
        throw TooFewElemException();

    std::vector<int> sorted(_storage);
    std::sort(sorted.begin(), sorted.end());

    long span = static_cast<long>(sorted[1]) - sorted[0];
    for (size_t i = 2; i < sorted.size(); i++)
        span = std::min(span, static_cast<long>(sorted[i]) - sorted[i - 1]);

    if (span > INT_MAX)
        throw SpanOverflowException();
    return (static_cast<int>(span));
}

int     Span::longestSpan() const {
    if (_storage.size() < 2)
        throw TooFewElemException();

    std::vector<long> values(_storage.begin(), _storage.end());
    long min = *std::min_element(values.begin(), values.end());
    long max = *std::max_element(values.begin(), values.end());

    if (max - min > INT_MAX)
        throw SpanOverflowException();
    return (static_cast<int>(max - min));
}

