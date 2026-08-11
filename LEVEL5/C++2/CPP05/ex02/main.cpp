#include <cstdlib>
#include <ctime>
#include <iostream>
#include "Bureaucrat.hpp"
#include "AForm.hpp"
#include "ShrubberyCreationForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "PresidentialPardonForm.hpp"
#include "utils.hpp"

#ifndef TOO_LOW
# define TOO_LOW 1
#endif

#ifndef TOO_HIGH
# define TOO_HIGH 1
#endif

#ifndef TEST
# define TEST  1
#endif

#ifndef SHRUBBERY
# define SHRUBBERY 1
#endif

#ifndef ROBOTOMY
# define ROBOTOMY 1
#endif

#ifndef PARDON
# define PARDON 1
#endif

int main(void) {

    std::srand(static_cast<unsigned int>(std::time(NULL)));

#if TEST == 1
    {
        std::cout << BOLD_GREEN << "=== ORTHODOX CANONICAL FORM ===" << endofline;
        ShrubberyCreationForm def;
        ShrubberyCreationForm garden("Garden");
        ShrubberyCreationForm copy(garden);

        std::cout << "Default constructor " << def << endofline;
        std::cout << "Target constructor  " << garden << endofline;
        std::cout << "Copy constructor    " << copy << endofline;

        std::cout << BOLD_GREEN << "=== BUREAUCRAT ===" << endofline;
        Bureaucrat boss("Boss", 3);
        std::cout << boss << endofline;
        boss.incrementGrade();
        std::cout << boss << endofline;
        boss.decrementGrade();
        std::cout << boss << endofline;

        std::cout << BOLD_GREEN << "=== SIGN OK ===" << endofline;
        boss.signForm(garden);
        std::cout << garden << endofline;

        // only _signed crosses the assignment: the three const members stay put
        def = garden;
        std::cout << "Assignment operator " << def << endofline;

        std::cout << BOLD_GREEN << "=== SIGN REFUSED ===" << endofline;
        Bureaucrat intern("Intern", 150);
        RobotomyRequestForm audit("Audit");
        intern.signForm(audit);
        std::cout << audit << endofline;

        std::cout << BOLD_GREEN << "=== beSigned THROWS ===" << endofline;
        try {
            audit.beSigned(intern);
        } catch (std::exception &e) {
            std::cerr << BOLD_WHITE << intern.getName() << ": " << e.what() << endofline;
        }

        std::cout << BOLD_GREEN << "=== POLYMORPHIC DELETE ===" << endofline;
        AForm* poly = new PresidentialPardonForm("Poly");
        delete poly;
    }
#endif

#if SHRUBBERY == 1
    {
        std::cout << BOLD_GREEN << "=== SHRUBBERY CREATION FORM (145 / 137) ===" << endofline;
        ShrubberyCreationForm garden("Garden");
        Bureaucrat gardener("Gardener", 137);
        Bureaucrat trainee("Trainee", 140);
        std::cout << garden << endofline;

        std::cout << BOLD_YELLOW << "-- execute before signing --" << endofline;
        gardener.executeForm(garden);

        std::cout << BOLD_YELLOW << "-- sign --" << endofline;
        gardener.signForm(garden);

        std::cout << BOLD_YELLOW << "-- execute with grade 140 (too low) --" << endofline;
        trainee.executeForm(garden);

        std::cout << BOLD_YELLOW << "-- execute with grade 137 (exact bound) --" << endofline;
        gardener.executeForm(garden);
    }
#endif

#if ROBOTOMY == 1
    {
        std::cout << BOLD_GREEN << "=== ROBOTOMY REQUEST FORM (72 / 45) ===" << endofline;
        RobotomyRequestForm robot("Bender");
        Bureaucrat surgeon("Surgeon", 45);
        Bureaucrat nurse("Nurse", 50);
        std::cout << robot << endofline;

        std::cout << BOLD_YELLOW << "-- execute before signing --" << endofline;
        surgeon.executeForm(robot);

        std::cout << BOLD_YELLOW << "-- sign --" << endofline;
        nurse.signForm(robot);

        std::cout << BOLD_YELLOW << "-- execute with grade 50 (too low) --" << endofline;
        nurse.executeForm(robot);

        std::cout << BOLD_YELLOW << "-- execute with grade 45 (exact bound), twice --" << endofline;
        surgeon.executeForm(robot);
        surgeon.executeForm(robot);
    }
#endif

#if PARDON == 1
    {
        std::cout << BOLD_GREEN << "=== PRESIDENTIAL PARDON FORM (25 / 5) ===" << endofline;
        PresidentialPardonForm pardon("Arthur Dent");
        Bureaucrat president("President", 5);
        Bureaucrat aide("Aide", 10);
        std::cout << pardon << endofline;

        std::cout << BOLD_YELLOW << "-- execute before signing --" << endofline;
        president.executeForm(pardon);

        std::cout << BOLD_YELLOW << "-- sign --" << endofline;
        aide.signForm(pardon);

        std::cout << BOLD_YELLOW << "-- execute with grade 10 (too low) --" << endofline;
        aide.executeForm(pardon);

        std::cout << BOLD_YELLOW << "-- execute with grade 5 (exact bound) --" << endofline;
        president.executeForm(pardon);
    }
#endif

#if TOO_LOW == 1
    {
        std::cout << BOLD_GREEN << "=== TOO LOW ===" << endofline;
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
    }
#endif

#if TOO_HIGH == 1
    {
        std::cout << BOLD_GREEN << "=== TOO HIGH ===" << endofline;
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
    }
#endif
    return (0);
}
