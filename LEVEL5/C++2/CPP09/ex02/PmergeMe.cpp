#include <iostream>
#include "PmergeMe.hpp"
#include "utils.hpp"

// ~TORS -------
PmergeMe::PmergeMe() : _vectorTimer(0), _dequeTimer(0) { }

PmergeMe::PmergeMe(const PmergeMe &src) { *this = src; }

PmergeMe& PmergeMe::operator= (const PmergeMe &other) {
    if (this != &other)
    {
        _vector = other._vector;
        _deque = other._deque;
        _vectorTimer = other._vectorTimer;
        _dequeTimer = other._dequeTimer;
    }
    return (*this);
}

PmergeMe::~PmergeMe() { }

// METHODS -------

// UTILS -------

// GETTERS / SETTERS -------

const std::vector<int>& PmergeMe::getVector(void) const { return (_vector); }

const std::deque<int>&  PmergeMe::getDeque(void) const { return (_deque); }

void    PmergeMe::setVector(const std::vector<int> &vector) { _vector = vector; }

void    PmergeMe::setDeque(const std::deque<int> &deque) { _deque = deque; }

double  PmergeMe::getVectorTimer(void) const { return (_vectorTimer); }

double  PmergeMe::getDequeTimer(void) const { return (_dequeTimer); }
