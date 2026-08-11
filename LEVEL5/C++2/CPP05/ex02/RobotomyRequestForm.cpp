#include <cstdlib>
#include <iostream>
#include "RobotomyRequestForm.hpp"
#include "Bureaucrat.hpp"
#include "utils.hpp"

RobotomyRequestForm::RobotomyRequestForm()
    : AForm("RobotomyRequestForm", 72, 45), _target("Default") {
    std::cout << BOLD_MAGENTA << "RobotomyRequestForm Default constructor called" << endofline;
}

RobotomyRequestForm::RobotomyRequestForm(std::string target)
    : AForm("RobotomyRequestForm", 72, 45), _target(target) {
    std::cout << BOLD_CYAN << "RobotomyRequestForm Target constructor called" << endofline;
}

RobotomyRequestForm::RobotomyRequestForm(const RobotomyRequestForm &src)
    : AForm(src), _target(src._target) {
    std::cout << BOLD_BLUE << "RobotomyRequestForm Copy constructor called" << endofline;
}

RobotomyRequestForm& RobotomyRequestForm::operator= (const RobotomyRequestForm &other) {
    std::cout << BOLD_BLUE << "RobotomyRequestForm Copy assignment operator called" << endofline;
    if (this != &other)
        AForm::operator=(other);
    return (*this);
}

RobotomyRequestForm::~RobotomyRequestForm() {
    std::cout << BOLD_RED << "RobotomyRequestForm Destructor called" << endofline;
}

std::string RobotomyRequestForm::getTarget() const { return (_target);}

void    RobotomyRequestForm::executeAction() const {
    std::cout << BOLD_WHITE << " *** Drilling noises ***" << endofline;
    unsigned int random = std::rand();
    if (random % 2 == 0)
        std::cout << BOLD_YELLOW << "Target " << _target << " has been well robotomized" << endofline;
    else
        std::cout << BOLD_RED << "There was an error during " << _target << "'s robotomy. Brain destroyed" << endofline;
}
