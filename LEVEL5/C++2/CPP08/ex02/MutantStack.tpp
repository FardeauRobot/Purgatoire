#ifndef MUTANTSTACK_TPP
# define MUTANTSTACK_TPP

#include "MutantStack.hpp"

# define TPL template <typename T>

TPL
MutantStack<T>::MutantStack() : std::stack<T>() {}

TPL
MutantStack<T>::MutantStack(const MutantStack &src) : std::stack<T>(src) {}

TPL
MutantStack<T>& MutantStack<T>::operator= (const MutantStack &other) {
    if (this != &other)
        std::stack<T>::operator=(other);
    return (*this);
}

TPL
MutantStack<T>::~MutantStack() {}

TPL
typename MutantStack<T>::iterator MutantStack<T>::begin() {
    return (this->c.begin());
}

TPL
typename MutantStack<T>::iterator MutantStack<T>::end() {
    return (this->c.end());
}

TPL
typename MutantStack<T>::const_iterator MutantStack<T>::begin() const {
    return (this->c.begin());
}

TPL
typename MutantStack<T>::const_iterator MutantStack<T>::end() const {
    return (this->c.end());
}

TPL
typename MutantStack<T>::reverse_iterator MutantStack<T>::rbegin() {
    return (this->c.rbegin());
}

TPL
typename MutantStack<T>::reverse_iterator MutantStack<T>::rend() {
    return (this->c.rend());
}

TPL
typename MutantStack<T>::const_reverse_iterator MutantStack<T>::rbegin() const {
    return (this->c.rbegin());
}

TPL
typename MutantStack<T>::const_reverse_iterator MutantStack<T>::rend() const {
    return (this->c.rend());
}

# undef TPL

#endif
