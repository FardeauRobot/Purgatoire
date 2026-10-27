#include <climits>
#include "RPN.hpp"
#include "errors.hpp"


// ~TORS -------
RPN::RPN() { }

RPN::RPN(const RPN &src) : _numbers(src._numbers) { }

RPN& RPN::operator= (const RPN &other) {
    if (this != &other)
        _numbers = other._numbers;
    return (*this);
}

RPN::~RPN() { }


// METHODS -------

void    RPN::apply(char operand) {

    int a;
    int b;

    if (std::string("+-*/").find(operand) == std::string::npos)
        F_ErrExit(std::string("Invalid token : ") + operand);
    if (_numbers.size() < 2)
        F_ErrExit("Too few numbers to do an operation");

    load(a,b);
    long res;

    switch (operand) {

        case '+': res = static_cast<long>(a) + b; break;

        case '-': res = static_cast<long>(a) - b; break;

        case '*': res = static_cast<long>(a) * b; break;

        default:
            if (b == 0)
                F_ErrExit("Can't divide by zero");
            res = static_cast<long>(a) / b; break;
    }
    if (res > INT_MAX || res < INT_MIN)
        F_ErrExit("Integer overflow");
    _numbers.push(static_cast<int>(res));
}

// UTILS -------
void    RPN::load(int &a, int &b) {
    b = _numbers.top();
    _numbers.pop();
    a = _numbers.top();
    _numbers.pop();


}

// GETTERS / SETTERS
RPN::Stack&  RPN::getStack(void) { return (_numbers); }
