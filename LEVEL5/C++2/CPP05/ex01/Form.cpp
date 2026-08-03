#include <iostream>

#include "Form.hpp"
#include "utils.hpp"

// ~TORS
Form::Form(): _name("Default"), _signed(false), _signGrade(75), _execGrade(75) {
    std::cout << BOLD_CYAN << "Form Default constructor called" << endofline;
}

Form::Form(std::string name, int signGrade, int execGrade):
    _name(name), _signed(false), _signGrade(signGrade), _execGrade(execGrade) {
    std::cout << BOLD_CYAN << "Form Name constructor called" << endofline;
    if (_signGrade < 1 || _execGrade < 1)
        throw GradeTooHighException();
    else if (_signGrade > 150 || _execGrade > 150)
        throw GradeTooLowException();
}

Form::Form(const Form &src):
    _name(src._name), _signed(src._signed),
    _signGrade(src._signGrade), _execGrade(src._execGrade) {
    std::cout << BOLD_BLUE << "Form Copy constructor called" << endofline;
}

Form& Form::operator= (const Form &other) {
    std::cout << BOLD_BLUE << "Form Copy assignment operator called" << endofline;
    if (this != &other) {
        // _name, _signGrade and _execGrade are const: only the status can change
        _signed = other._signed;
    }
    return (*this);
}

Form::~Form() {
    std::cout << BOLD_RED << "Form Destructor called" << endofline;
}

// METHODS
void Form::beSigned(const Bureaucrat &bureaucrat) {
    if (bureaucrat.getGrade() > _signGrade)
        throw GradeTooLowException();
    _signed = true;
}

// EXCEPTIONS
const char* Form::GradeTooHighException::what() const throw() {
    return "Form: grade too high";
}

const char* Form::GradeTooLowException::what() const throw() {
    return "Form: grade too low";
}

// GETTERS
std::string Form::getName() const { return (_name); }
bool        Form::getSigned() const { return (_signed); }
int         Form::getSignGrade() const { return (_signGrade); }
int         Form::getExecGrade() const { return (_execGrade); }

// FREE
std::ostream &operator<<(std::ostream &out, const Form &form) {
    out << "Form " << form.getName()
        << " [" << (form.getSigned() ? "signed" : "unsigned") << "]"
        << ", sign grade " << form.getSignGrade()
        << ", exec grade " << form.getExecGrade();
    return (out);
}
