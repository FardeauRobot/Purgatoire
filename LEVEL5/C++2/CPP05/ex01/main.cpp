#include "Bureaucrat.hpp"
#include "Form.hpp"
#include "utils.hpp"

#ifndef TOO_LOW
# define TOO_LOW 1
#endif

#ifndef TOO_HIGH
# define TOO_HIGH 1
#endif

int main(void) {
    Form def;
    Form taxes("Taxes", 50, 25);
    Form copy(taxes);

    std::cout << "Default constructor " << def << endofline;
    std::cout << "Copy constructor " << copy << endofline;
    std::cout << taxes << endofline;

    std::cout << BOLD_GREEN << "=== BUREAUCRAT === " << endofline;
    Bureaucrat boss("Boss", 3);
    std::cout << boss << endofline;
    boss.incrementGrade();
    std::cout << boss << endofline;
    boss.decrementGrade();
    std::cout << boss << endofline;

    std::cout << BOLD_GREEN << "=== SIGN OK === " << endofline;
    boss.signForm(taxes);
    std::cout << taxes << endofline;

    def = taxes;
    std::cout << "Assignment operator " << def << endofline;

    std::cout << BOLD_GREEN << "=== SIGN BOUNDARY === " << endofline;
    Bureaucrat clerk("Clerk", 50);
    Form permit("Permit", 50, 25);
    clerk.signForm(permit);
    std::cout << permit << endofline;

    std::cout << BOLD_GREEN << "=== SIGN REFUSED === " << endofline;
    Bureaucrat intern("Intern", 150);
    Form audit("Audit", 50, 25);
    intern.signForm(audit);
    std::cout << audit << endofline;

    std::cout << BOLD_GREEN << "=== beSigned THROWS === " << endofline;
    try {
        audit.beSigned(intern);
    } catch (std::exception &e) {
        std::cerr << BOLD_WHITE << intern.getName() << ": " << e.what() << endofline;
    }

#if TOO_LOW == 1
    std::cout << BOLD_GREEN << "=== TOO LOW === " << endofline;
    try {
        Form f("SignLow", 151, 25);
        std::cout << f << endofline;
    } catch (std::exception &e) {
        std::cerr << BOLD_WHITE << e.what() << endofline;
    }

    try {
        Form f("ExecLow", 50, 151);
        std::cout << f << endofline;
    } catch (std::exception &e) {
        std::cerr << BOLD_WHITE << e.what() << endofline;
    }

    try {
        Bureaucrat b("Bob", 151);
        std::cout << b << endofline;
    } catch (std::exception &e) {
        std::cerr << BOLD_WHITE << e.what() << endofline;
    }

    try {
        Bureaucrat c("Carl", 150);
        c.decrementGrade();
    } catch (std::exception &e) {
        std::cerr << BOLD_WHITE << e.what() << endofline;
    }
#endif

#if TOO_HIGH == 1
    std::cout << BOLD_GREEN << "=== TOO HIGH === " << endofline;
    try {
        Form f("SignHigh", 0, 25);
        std::cout << f << endofline;
    } catch (std::exception &e) {
        std::cerr << BOLD_WHITE << e.what() << endofline;
    }

    try {
        Form f("ExecHigh", 50, 0);
        std::cout << f << endofline;
    } catch (std::exception &e) {
        std::cerr << BOLD_WHITE << e.what() << endofline;
    }

    try {
        Bureaucrat d("Dan", 0);
        std::cout << d << endofline;
    } catch (std::exception &e) {
        std::cerr << BOLD_WHITE << e.what() << endofline;
    }

    try {
        Bureaucrat ev("Eve", 1);
        ev.incrementGrade();
    } catch (std::exception &e) {
        std::cerr << BOLD_WHITE << e.what() << endofline;
    }
#endif
    return (0);
}
