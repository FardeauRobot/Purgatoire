#include <cstdlib>
#include <iostream>

#include "RobotomyRequestForm.hpp"
#include "utils.hpp"

// ~TORS
RobotomyRequestForm::RobotomyRequestForm():
    AForm("RobotomyRequestForm", 72, 45), _target("Default") {
    std::cout << BOLD_CYAN << "RobotomyRequestForm Default constructor called" << endofline;
}

RobotomyRequestForm::RobotomyRequestForm(std::string target):
    AForm("RobotomyRequestForm", 72, 45), _target(target) {
    std::cout << BOLD_CYAN << "RobotomyRequestForm Target constructor called" << endofline;
}

RobotomyRequestForm::RobotomyRequestForm(const RobotomyRequestForm &src):
    AForm(src), _target(src._target) {
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

// METHODS
void RobotomyRequestForm::executeAction() const {
    std::cout << BOLD_WHITE << "* BZZZZZZT DRRRRRRR BZZZZZZT *" << endofline;
    if (std::rand() % 2 == 0)
        std::cout << BOLD_GREEN << _target << " has been robotomized successfully" << endofline;
    else
        std::cout << BOLD_RED << "the robotomy of " << _target << " failed" << endofline;
}

// GETTERS
std::string RobotomyRequestForm::getTarget() const { return (_target); }
