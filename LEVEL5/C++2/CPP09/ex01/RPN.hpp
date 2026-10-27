#ifndef RPN_HPP
# define RPN_HPP

#include <list>
#include <stack>

#include <iostream>

class RPN {
    public:
        typedef std::stack<int, std::list<int> > Stack;

        RPN();
        RPN(const RPN &src);
        RPN& operator= (const RPN &other);
        ~RPN();
        
        void apply(char operand);
        Stack&  getStack(void);

    private:
        Stack   _numbers;
        void load(int &a, int &b);

};

#endif
