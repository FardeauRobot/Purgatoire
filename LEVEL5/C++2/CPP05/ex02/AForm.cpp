#include <iostream>

#include "AForm.hpp"
#include "utils.hpp"

// ~TORS
AForm::AForm(): _name("Default"), _signed(false), _signGrade(75), _execGrade(75) {
    std::cout << BOLD_CYAN << "AForm Default constructor called" << endofline;
}

AForm::AForm(std::string name, int signGrade, int execGrade):
    _name(name), _signed(false), _signGrade(signGrade), _execGrade(execGrade) {
    std::cout << BOLD_CYAN << "AForm Name constructor called" << endofline;
    if (_signGrade < 1 || _execGrade < 1)
        throw GradeTooHighException();
    else if (_signGrade > 150 || _execGrade > 150)
        throw GradeTooLowException();
}

AForm::AForm(const AForm &src):
    _name(src._name), _signed(src._signed),
    _signGrade(src._signGrade), _execGrade(src._execGrade) {
    std::cout << BOLD_BLUE << "AForm Copy constructor called" << endofline;
}

AForm& AForm::operator= (const AForm &other) {
    std::cout << BOLD_BLUE << "AForm Copy assignment operator called" << endofline;
    if (this != &other) {
        // _name, _signGrade and _execGrade are const: only the status can change
        _signed = other._signed;
    }
    return (*this);
}

AForm::~AForm() {
    std::cout << BOLD_RED << "AForm Destructor called" << endofline;
}

// METHODS
void AForm::beSigned(const Bureaucrat &bureaucrat) {
    if (bureaucrat.getGrade() > _signGrade)
        throw GradeTooLowException();
    _signed = true;
}

void AForm::execute(const Bureaucrat &executor) const {
    if (!_signed)
        throw FormNotSignedException();
    if (executor.getGrade() > _execGrade)
        throw GradeTooLowException();
    executeAction();
}

// EXCEPTIONS
const char* AForm::GradeTooHighException::what() const throw() {
    return "AForm: grade too high";
}

const char* AForm::GradeTooLowException::what() const throw() {
    return "AForm: grade too low";
}

const char* AForm::FormNotSignedException::what() const throw() {
    return "AForm: form is not signed";
}

// GETTERS
std::string AForm::getName() const { return (_name); }
bool        AForm::getSigned() const { return (_signed); }
int         AForm::getSignGrade() const { return (_signGrade); }
int         AForm::getExecGrade() const { return (_execGrade); }

// FREE
std::ostream &operator<<(std::ostream &out, const AForm &form) {
    out << "AForm " << form.getName()
        << " [" << (form.getSigned() ? "signed" : "unsigned") << "]"
        << ", sign grade " << form.getSignGrade()
        << ", exec grade " << form.getExecGrade();
    return (out);
}
