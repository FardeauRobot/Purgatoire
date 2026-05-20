#include "Test.hpp"
#include "utils.hpp"
#include <iostream>

int main(void) {
    std::string O_StringTest = "HI THIS IS BRAIN";
    std::string *stringPTR = &O_StringTest;
    std::string &stringREF = O_StringTest;

    std::cout << BOLD_CYAN << "This is O_StringTest's value OG = " << O_StringTest << endofline;
    std::cout << BOLD_GREEN << "This is O_StringTest's value PTR = " << *stringPTR << endofline;
    std::cout << BOLD_BLUE << "This is O_StringTest's value REF = " << stringREF << "\n\n" << endofline;


    std::cout << BOLD_CYAN << "This is O_StringTest's address OG = " << &O_StringTest << endofline;
    std::cout << BOLD_GREEN << "This is O_StringTest's address PTR = " << stringPTR << endofline;
    std::cout << BOLD_BLUE << "This is O_StringTest's address REF = " << &stringREF << endofline;
    return 0;
}
