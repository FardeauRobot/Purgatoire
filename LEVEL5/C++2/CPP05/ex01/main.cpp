#include <iostream>

#include "Bureaucrat.hpp"
#include "Form.hpp"
#include "utils.hpp"

static void title(const std::string &text) {
    std::cout << BOLD_MAGENTA << "\n=== " << text << " ===" << endofline;
}

int main(void) {
    title("A form refuses invalid grades at construction");
    try {
        Form bad("Impossible", 0, 42);
        std::cout << BOLD_YELLOW << bad << endofline;
    } catch (std::exception &e) {
        std::cout << BOLD_RED << "caught: " << e.what() << endofline;
    }

    try {
        Form bad("Impossible", 42, 151);
        std::cout << BOLD_YELLOW << bad << endofline;
    } catch (std::exception &e) {
        std::cout << BOLD_RED << "caught: " << e.what() << endofline;
    }

    title("A bureaucrat whose grade is too low cannot sign");
    {
        Bureaucrat intern("Milton", 148);
        Form taxes("Tax Return 28B", 100, 50);

        std::cout << BOLD_YELLOW << intern << endofline;
        std::cout << BOLD_YELLOW << taxes << endofline;
        intern.signForm(taxes);
        std::cout << BOLD_YELLOW << taxes << endofline;
    }

    title("A bureaucrat whose grade is high enough signs");
    {
        Bureaucrat boss("Lumbergh", 50);
        Form taxes("Tax Return 28B", 100, 50);

        boss.signForm(taxes);
        std::cout << BOLD_YELLOW << taxes << endofline;
    }

    title("Promoting a bureaucrat until it becomes possible");
    {
        Bureaucrat clerk("Peter", 101);
        Form taxes("Tax Return 28B", 100, 50);

        clerk.signForm(taxes);
        clerk.gradeIncrease();
        std::cout << BOLD_YELLOW << clerk << endofline;
        clerk.signForm(taxes);
    }

    title("beSigned() thrown outside of signForm() is still catchable");
    {
        Bureaucrat clerk("Peter", 101);
        Form taxes("Tax Return 28B", 100, 50);

        try {
            taxes.beSigned(clerk);
        } catch (Form::GradeTooLowException &e) {
            std::cout << BOLD_RED << "caught by exact type: " << e.what() << endofline;
        }
    }

    return 0;
}
