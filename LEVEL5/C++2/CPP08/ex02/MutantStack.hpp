#ifndef MUTANTSTACK_HPP
# define MUTANTSTACK_HPP

#include <iostream>
#include <stack>

class MutantStack : public std::stack {
    public:
        MutantStack();
        MutantStack(const MutantStack &src);
        MutantStack& operator= (const MutantStack &other);
        ~MutantStack();
};

#endif
