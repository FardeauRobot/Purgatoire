#include <cstdlib>
#include <ctime>
#include <iostream>

#include "AForm.hpp"
#include "Bureaucrat.hpp"
#include "PresidentialPardonForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "ShrubberyCreationForm.hpp"
#include "utils.hpp"

static void title(const std::string &text) {
    std::cout << BOLD_MAGENTA << "\n=== " << text << " ===" << endofline;
}

int main(void) {
    std::srand(static_cast<unsigned int>(std::time(NULL)));

    title("An unsigned form cannot be executed");
    {
        Bureaucrat boss("Zaphod", 1);
        ShrubberyCreationForm garden("home");

        boss.executeForm(garden);
    }

    title("A signed form still needs the exec grade");
    {
        Bureaucrat clerk("Milton", 140);
        ShrubberyCreationForm garden("home");

        clerk.signForm(garden);
        clerk.executeForm(garden);
    }

    title("Sign then execute for real");
    {
        Bureaucrat boss("Zaphod", 1);
        ShrubberyCreationForm garden("home");

        boss.signForm(garden);
        boss.executeForm(garden);
        std::cout << BOLD_YELLOW << "check the file ./home_shrubbery" << endofline;
    }

    title("Robotomy: 50% success, run it a few times");
    {
        Bureaucrat boss("Zaphod", 1);

        for (int i = 0; i < 4; i++) {
            RobotomyRequestForm request("Bender");
            boss.signForm(request);
            boss.executeForm(request);
        }
    }

    title("Presidential pardon");
    {
        Bureaucrat boss("Zaphod", 1);
        Bureaucrat clerk("Milton", 24);
        PresidentialPardonForm pardon("Arthur Dent");

        clerk.signForm(pardon);
        clerk.executeForm(pardon);
        boss.executeForm(pardon);
    }

    title("Polymorphism: the same AForm& drives different actions");
    {
        Bureaucrat boss("Zaphod", 1);
        RobotomyRequestForm request("Marvin");
        PresidentialPardonForm pardon("Ford Prefect");
        AForm *forms[2];

        forms[0] = &request;
        forms[1] = &pardon;
        for (int i = 0; i < 2; i++) {
            std::cout << BOLD_YELLOW << *forms[i] << endofline;
            boss.signForm(*forms[i]);
            boss.executeForm(*forms[i]);
        }
    }

    return 0;
}
