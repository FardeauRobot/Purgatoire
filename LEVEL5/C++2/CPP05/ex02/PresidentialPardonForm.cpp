#include <iostream>
#include "PresidentialPardonForm.hpp"
#include "Bureaucrat.hpp"
#include "utils.hpp"

PresidentialPardonForm::PresidentialPardonForm()
    : AForm("PresidentialPardonForm", 25, 5), _target("Default") {
    std::cout << BOLD_MAGENTA << "PresidentialPardonForm Default constructor called" << endofline;
}

PresidentialPardonForm::PresidentialPardonForm(std::string target)
    : AForm("PresidentialPardonForm", 25, 5), _target(target) {
    std::cout << BOLD_CYAN << "PresidentialPardonForm Target constructor called" << endofline;
}

PresidentialPardonForm::PresidentialPardonForm(const PresidentialPardonForm &src)
    : AForm(src), _target(src._target) {
    std::cout << BOLD_BLUE << "PresidentialPardonForm Copy constructor called" << endofline;
}

PresidentialPardonForm& PresidentialPardonForm::operator= (const PresidentialPardonForm &other) {
    std::cout << BOLD_BLUE << "PresidentialPardonForm Copy assignment operator called" << endofline;
    if (this != &other)
        AForm::operator=(other);
    return (*this);
}

PresidentialPardonForm::~PresidentialPardonForm() {
    std::cout << BOLD_RED << "PresidentialPardonForm Destructor called" << endofline;
}

std::string PresidentialPardonForm::getTarget() const { return (_target);}

void    PresidentialPardonForm::executeAction() const {
    std::cout << BOLD_GREEN << _target << " has been pardoned by Zaphod Beeblebrox" << endofline;
}
