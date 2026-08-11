#include <iostream>
#include "Bureaucrat.hpp"
#include "AForm.hpp"
#include "utils.hpp"

Bureaucrat::Bureaucrat(): _name("Default"), _grade(150) {
    std::cout << BOLD_MAGENTA << "Bureaucrat Default constructor called" << endofline;
}

Bureaucrat::Bureaucrat(std::string name, int grade): _name(name), _grade(grade) {
    std::cout << BOLD_CYAN << "Bureaucrat Name constructor called" << endofline;
    if (_grade > 150)
        throw GradeTooLowException(_name);
    else if (_grade < 1)
        throw GradeTooHighException(_name);
}

Bureaucrat::Bureaucrat(const Bureaucrat &src) : _name(src._name), _grade(src._grade) {
    std::cout << BOLD_BLUE << "Bureaucrat Copy constructor called" << endofline;
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

std::string Bureaucrat::getName() const { return (_name);}
int Bureaucrat::getGrade() const { return (_grade);}

void    Bureaucrat::incrementGrade() {
    if (_grade > 1)
        _grade--;
    else
        throw GradeTooHighException(_name);
}

void    Bureaucrat::decrementGrade() {
    if (_grade < 150)
        _grade++;
    else
        throw GradeTooLowException(_name);
}

Bureaucrat::GradeTooHighException::GradeTooHighException(const std::string &name) : _msg (name + "'s grade is too high, must be at least 1.") {}
Bureaucrat::GradeTooHighException::~GradeTooHighException() throw() {}

const char * Bureaucrat::GradeTooHighException::what() const throw () {return (_msg.c_str());}

Bureaucrat::GradeTooLowException::GradeTooLowException(const std::string &name) : _msg (name + "'s grade is too low, must be at most 150.") {}
Bureaucrat::GradeTooLowException::~GradeTooLowException() throw() {}

const char * Bureaucrat::GradeTooLowException::what() const throw () {return (_msg.c_str());}

void    Bureaucrat::signForm(AForm& form) {
    try {
        form.beSigned(*this);
        std::cout << BOLD_GREEN << _name << " signed " << form.getName() << endofline;
    } catch (std::exception &e) {
        std::cout << BOLD_RED << _name << " couldn't sign " << form.getName()
                  << " because " << e.what() << endofline;
    }
}

void    Bureaucrat::executeForm(AForm const& form) const {
    try {
        form.execute(*this);
        std::cout << BOLD_GREEN << _name << " executed " << form.getName() << endofline;
    } catch (std::exception &e) {
        std::cout << BOLD_RED << _name << " couldn't execute " << form.getName()
                  << " because " << e.what() << endofline;
    }
}

std::ostream& operator<< (std::ostream& os, Bureaucrat const& bureaucrat) {
    os << BOLD_CYAN << bureaucrat.getName() << RESET << ", bureaucrat grade " << BOLD_YELLOW << bureaucrat.getGrade() << RESET;
    return (os);
}