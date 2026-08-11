#include <iostream>
#include "AForm.hpp"
#include "Bureaucrat.hpp"
#include "utils.hpp"

AForm::AForm(): _name("Default"), _signed(false), _gradeToSign(150), _gradeToExecute(150) {
    std::cout << BOLD_MAGENTA << "AForm Default constructor called" << endofline;
}

AForm::AForm(std::string name, int gradeToSign, int gradeToExecute)
    : _name(name), _signed(false), _gradeToSign(gradeToSign), _gradeToExecute(gradeToExecute) {
    std::cout << BOLD_CYAN << "AForm Name constructor called" << endofline;
    if (_gradeToSign > 150 || _gradeToExecute > 150)
        throw GradeTooLowException(_name + "'s required grade is too low, must be at most 150.");
    else if (_gradeToSign < 1 || _gradeToExecute < 1)
        throw GradeTooHighException(_name + "'s required grade is too high, must be at least 1.");
}

AForm::AForm(const AForm &src)
    : _name(src._name), _signed(src._signed),
      _gradeToSign(src._gradeToSign), _gradeToExecute(src._gradeToExecute) {
    std::cout << BOLD_BLUE << "AForm Copy constructor called" << endofline;
}

AForm& AForm::operator= (const AForm &other) {
    std::cout << BOLD_BLUE << "AForm Copy assignment operator called" << endofline;
    if (this != &other)
        _signed = other._signed;
    return (*this);
}

AForm::~AForm() {
    std::cout << BOLD_RED << "AForm Destructor called" << endofline;
}

std::string AForm::getName() const { return (_name);}
bool        AForm::getSigned() const { return (_signed);}
int         AForm::getGradeToSign() const { return (_gradeToSign);}
int         AForm::getGradeToExecute() const { return (_gradeToExecute);}

AForm::GradeTooHighException::GradeTooHighException(const std::string &msg) : _msg (msg) {}
AForm::GradeTooHighException::~GradeTooHighException() throw() {}

const char * AForm::GradeTooHighException::what() const throw () {return (_msg.c_str());}

AForm::GradeTooLowException::GradeTooLowException(const std::string &msg) : _msg (msg) {}
AForm::GradeTooLowException::~GradeTooLowException() throw() {}

const char * AForm::GradeTooLowException::what() const throw () {return (_msg.c_str());}

AForm::FormNotSignedException::FormNotSignedException(const std::string &msg) : _msg (msg) {}
AForm::FormNotSignedException::~FormNotSignedException() throw() {}

const char * AForm::FormNotSignedException::what() const throw () {return (_msg.c_str());}

void    AForm::beSigned(const Bureaucrat &bureaucrat) {
    if (bureaucrat.getGrade() <= _gradeToSign)
        _signed = true;
    else
        throw GradeTooLowException("grade too low to sign");
}

void    AForm::execute(const Bureaucrat &executor) const {
    if (!_signed)
        throw FormNotSignedException(_name + " is not signed");
    if (executor.getGrade() > _gradeToExecute)
        throw GradeTooLowException(executor.getName() + "'s grade is too low to execute " + _name);
    executeAction();
}

std::ostream& operator<< (std::ostream& os, AForm const& form) {
    os << BOLD_CYAN << form.getName() << RESET << ", form "
       << (form.getSigned() ? BOLD_GREEN "signed" : BOLD_RED "unsigned") << RESET
       << ", grade " << BOLD_YELLOW << form.getGradeToSign() << RESET << " to sign, grade "
       << BOLD_YELLOW << form.getGradeToExecute() << RESET << " to execute";
    return (os);
}
