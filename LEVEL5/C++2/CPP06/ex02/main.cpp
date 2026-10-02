#include <iostream>
#include <cstdlib>
#include <ctime>
#include "functions.hpp"
#include "utils.hpp"

int main(void) {
    std::srand(static_cast<unsigned int>(std::time(NULL)));

    for (int i = 0; i < 6; ++i) {
        std::cout << BOLD_BLUE << "TEST NB " << i << endofline;
        Base *p = generate();

        std::cout << BOLD_YELLOW << "POINTER" << endofline;
        identify(p);
        std::cout << BOLD_GREEN <<  "REFERENCE" << endofline;
        identify(*p);
        std::cout << endofline;

        delete p;
    }
    return 0;
}
