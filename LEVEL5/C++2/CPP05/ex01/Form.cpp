#include <iostream>
#include "Form.hpp"
#include "Bureaucrat.hpp"
#include "utils.hpp"

Form::Form(): _name("Default"), _signed(false), _gradeToSign(150), _gradeToExecute(150) {
    std::cout << BOLD_MAGENTA << "Form Default constructor called" << endofline;
}

Form::Form(std::string name, int gradeToSign, int gradeToExecute)
    : _name(name), _signed(false), _gradeToSign(gradeToSign), _gradeToExecute(gradeToExecute) {
    std::cout << BOLD_CYAN << "Form Name constructor called" << endofline;
    if (_gradeToSign > 150 || _gradeToExecute > 150)
        throw GradeTooLowException(_name + "'s required grade is too low, must be at most 150.");
    else if (_gradeToSign < 1 || _gradeToExecute < 1)
        throw GradeTooHighException(_name + "'s required grade is too high, must be at least 1.");
}

Form::Form(const Form &src)
    : _name(src._name), _signed(src._signed),
      _gradeToSign(src._gradeToSign), _gradeToExecute(src._gradeToExecute) {
    std::cout << BOLD_BLUE << "Form Copy constructor called" << endofline;
}

Form& Form::operator= (const Form &other) {
    std::cout << BOLD_BLUE << "Form Copy assignment operator called" << endofline;
    if (this != &other)
        _signed = other._signed;
    return (*this);
}

Form::~Form() {
    std::cout << BOLD_RED << "Form Destructor called" << endofline;
}

std::string Form::getName() const { return (_name);}
bool        Form::getSigned() const { return (_signed);}
int         Form::getGradeToSign() const { return (_gradeToSign);}
int         Form::getGradeToExecute() const { return (_gradeToExecute);}

Form::GradeTooHighException::GradeTooHighException(const std::string &msg) : _msg (msg) {}
Form::GradeTooHighException::~GradeTooHighException() throw() {}

const char * Form::GradeTooHighException::what() const throw () {return (_msg.c_str());}

Form::GradeTooLowException::GradeTooLowException(const std::string &msg) : _msg (msg) {}
Form::GradeTooLowException::~GradeTooLowException() throw() {}

const char * Form::GradeTooLowException::what() const throw () {return (_msg.c_str());}

void    Form::beSigned(const Bureaucrat &bureaucrat) {
    if (bureaucrat.getGrade() <= _gradeToSign)
        _signed = true;
    else 
        throw GradeTooLowException("grade too low to sign");
}

std::ostream& operator<< (std::ostream& os, Form const& form) {
    os << BOLD_CYAN << form.getName() << RESET << ", form "
       << (form.getSigned() ? BOLD_GREEN "signed" : BOLD_RED "unsigned") << RESET
       << ", grade " << BOLD_YELLOW << form.getGradeToSign() << RESET << " to sign, grade "
       << BOLD_YELLOW << form.getGradeToExecute() << RESET << " to execute";
    return (os);
}
