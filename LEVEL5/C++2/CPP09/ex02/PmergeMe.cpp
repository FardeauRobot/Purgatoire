#include <iostream>
#include <cstdlib>
#include <ctime>
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

// The subject times data management too, so filling happens inside the timer.
void    PmergeMe::sortVector(int argc, char **argv) {
    std::clock_t    start = std::clock();

    for (int i = 1; i < argc; ++i)
        _vector.push_back(static_cast<int>(std::strtol(argv[i], NULL, 10)));
    _sortVector(1);
    _vectorTimer = static_cast<double>(std::clock() - start) / CLOCKS_PER_SEC * 1000000;
}

void    PmergeMe::sortDeque(int argc, char **argv) {
    std::clock_t    start = std::clock();

    for (int i = 1; i < argc; ++i)
        _deque.push_back(static_cast<int>(std::strtol(argv[i], NULL, 10)));
    _sortDeque(1);
    _dequeTimer = static_cast<double>(std::clock() - start) / CLOCKS_PER_SEC * 1000000;
}

void    PmergeMe::_sortVector(size_t blockSize) { (void)blockSize; }

void    PmergeMe::_sortDeque(size_t blockSize) { (void)blockSize; }

// UTILS -------

// GETTERS / SETTERS -------

const std::vector<int>& PmergeMe::getVector(void) const { return (_vector); }

const std::deque<int>&  PmergeMe::getDeque(void) const { return (_deque); }

double  PmergeMe::getVectorTimer(void) const { return (_vectorTimer); }

double  PmergeMe::getDequeTimer(void) const { return (_dequeTimer); }
