#include <iostream>
#include "Bureaucrat.hpp"
#include "utils.hpp"

Bureaucrat::Bureaucrat(std::string name, int grade): _name(name), _grade(grade) {
    std::cout << BOLD_CYAN << "Bureaucrat Name constructor called" << endofline;
    if (_grade > 150)
        throw GradeTooLowException();
    else if (_grade < 1)
        throw GradeTooHighException();
}

Bureaucrat::Bureaucrat(const Bureaucrat &src) {
    std::cout << BOLD_BLUE << "Bureaucrat Copy constructor called" << endofline;
    *this = src;
}

Bureaucrat& Bureaucrat::operator= (const Bureaucrat &other) {
    std::cout << BOLD_BLUE << "Bureaucrat Copy assignment operator called" << endofline;
    if (this != &other)
        _grade = other._grade;
    return (*this);
}

Bureaucrat::~Bureaucrat() {
    std::cout << BOLD_RED << "Bureaucrat Destructor called" << endofline;
}

Bureaucrat::GradeTooHighException::GradeTooHighException(const std::string &name) :
_msg (_name + "'s grade is too high, must be at maximum 1.") {}

const char * Bureaucrat::GradeTooHighException::what() const throw () {return (_msg.c_str());}

Bureaucrat::GradeTooLowException::GradeTooLowException(const std::string &name) :
_msg (_name + "'s grade is too low, must be at least 150.") {}

const char * Bureaucrat::GradeTooLowException::what() const throw () {return (_msg.c_str());}