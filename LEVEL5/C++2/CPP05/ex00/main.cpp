#include "Bureaucrat.hpp"
#include "utils.hpp"

#ifndef TOO_LOW
# define TOO_LOW 1
#endif

#ifndef TOO_HIGH
# define TOO_HIGH 1
#endif

int main(void) {
    Bureaucrat def;
    Bureaucrat tom("Tom", 42);
    Bureaucrat copy(tom);

    std::cout << "Default constructor " << def << endofline;
    def = tom;
    std::cout << "Assignment operator " << def << endofline;
    std::cout << "Copy constructor " << copy << endofline;
    std::cout <<  tom << endofline;
    tom.incrementGrade();
    std::cout << tom << endofline;
    tom.decrementGrade();
    std::cout << tom << endofline;

#if TOO_LOW == 1
    std::cout << BOLD_GREEN << "=== TOO LOW === " << endofline;
    try {
        Bureaucrat b("Bob", 151);
        std::cout << b << endofline;
    } catch (std::exception &e) {
        std::cerr << BOLD_WHITE << e.what() << endofline;
    }

    try {
        Bureaucrat c("Carl", 150);
        std::cout << c << endofline;
        c.decrementGrade();
    } catch (std::exception &e) {
        std::cerr << BOLD_WHITE << e.what() << endofline;
    }
#endif

#if TOO_HIGH == 1
    std::cout << BOLD_GREEN << "=== TOO HIGH === " << endofline;
    try {
        Bureaucrat d("Dan", 0);
        std::cout << d << endofline;
    } catch (std::exception &e) {
        std::cerr << BOLD_WHITE << e.what() << endofline;
    }

    try {
        Bureaucrat ev("Eve", 1);
        std::cout << ev << endofline;
        ev.incrementGrade();
    } catch (std::exception &e) {
        std::cerr << BOLD_WHITE << e.what() << endofline;
    }
#endif
    return (0);
}
