#include <iostream>
#include <cstdlib>
#include "functions.hpp"
#include "A.hpp"
#include "B.hpp"
#include "C.hpp"
#include "utils.hpp"

Base *generate(void) {
    switch (std::rand() % 3) {
        case 0: {
            std::cout << BOLD_WHITE << "Generated a type A object" << endofline;
            return (new A());
        }
        case 1: {
            std::cout << BOLD_WHITE << "Generated a type B object" << endofline;
            return (new B());
        }
        default: {
            std::cout << BOLD_WHITE << "Generated a type C object" << endofline;
            return (new C());
        }
    }
}

void identify(Base *p) {
    if (dynamic_cast<A *>(p) != NULL)
        std::cout << "Is a pointer to type A object" << endofline;
    else if (dynamic_cast<B *>(p) != NULL)
        std::cout << "Is a pointer to type B object" << endofline;
    else if (dynamic_cast<C *>(p) != NULL)
        std::cout << "Is a pointer to type C object" << endofline;
    else
        std::cout << "Unknown" << endofline;
}

void identify(Base &p) {
    try {
        A &a = dynamic_cast<A &>(p);
        (void)a;
        std::cout << "Is a reference to type A object" << endofline;
        return;
    } catch (...) { std::cout << "Not a reference to a type A object" << endofline;}
    try {
        B &b = dynamic_cast<B &>(p);
        (void)b;
        std::cout << "Is a reference to type B object" << endofline;
        return;
    } catch (...) {std::cout << "Not a reference to a type B object" << endofline;}
    try {
        C &c = dynamic_cast<C &>(p);
        (void)c;
        std::cout << "Is a reference to type C object" << endofline;
        return;
    } catch (...) {std::cout << "Not a reference to a type C object" << endofline;}
    std::cout << "Unknown" << endofline;
}
