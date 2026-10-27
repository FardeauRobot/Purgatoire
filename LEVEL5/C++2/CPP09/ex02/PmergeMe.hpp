#ifndef PMERGEME_HPP
# define PMERGEME_HPP

#include <iostream>
#include <vector>
#include <deque>
#include <ctime>

#include "utils.hpp"


template<typename T>
void    IMSort(T &container, double &timer) {
        (void)container;
        (void)timer;
}

template<typename T>
void    Print(const T &container) {
    for (typename T::const_iterator it = container.begin(); it != container.end(); ++it)
    {
        if (it != container.begin())
            std::cout << " ";
        std::cout << *it;
    }
    std::cout << endofline;
}

class PmergeMe {

        std::vector<int>    _vector;
        std::deque<int>    _deque;

        double  _vectorTimer;
        double  _dequeTimer;

    public:
        PmergeMe();
        PmergeMe(const PmergeMe &src);
        PmergeMe& operator= (const PmergeMe &other);
        ~PmergeMe();

        const std::vector<int>& getVector(void) const;
        const std::deque<int>&  getDeque(void) const;
        void    setVector(const std::vector<int> &vector);
        void    setDeque(const std::deque<int> &deque);
        double  getVectorTimer(void) const;
        double  getDequeTimer(void) const;
};


#endif
