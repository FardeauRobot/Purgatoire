#include <iostream>
#include "MutantStack.hpp"
#include "utils.hpp"

MutantStack::MutantStack() {
    std::cout << BOLD_CYAN << "MutantStack Default constructor called" << endofline;
}

MutantStack::MutantStack(const MutantStack &src) {
    std::cout << BOLD_BLUE << "MutantStack Copy constructor called" << endofline;
    *this = src;
}

MutantStack& MutantStack::operator= (const MutantStack &other) {
    std::cout << BOLD_BLUE << "MutantStack Copy assignment operator called" << endofline;
    (void)other;
    return (*this);
}

MutantStack::~MutantStack() {
    std::cout << BOLD_RED << "MutantStack Destructor called" << endofline;
}
