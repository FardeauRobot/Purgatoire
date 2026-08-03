#include <iostream>

#include "Bureaucrat.hpp"
#include "utils.hpp"

// ~TORS
Bureaucrat::Bureaucrat(): _name("Default"), _grade(75) {
    std::cout << BOLD_CYAN << "Bureaucrat Default constructor called" << endofline;
}

Bureaucrat::Bureaucrat(std::string name, int grade): _name(name), _grade(grade) {
    std::cout << BOLD_CYAN << "Bureaucrat Name constructor called" << endofline;
    if (_grade < 1)
        throw GradeTooHighException();
    else if (_grade > 150)
        throw GradeTooLowException();
}

Bureaucrat::Bureaucrat(const Bureaucrat &src): _name(src._name), _grade(src._grade) {
    std::cout << BOLD_BLUE << "Bureaucrat Copy constructor called" << endofline;
}

Bureaucrat& Bureaucrat::operator= (const Bureaucrat &other) {
    std::cout << BOLD_BLUE << "Bureaucrat Copy assignment operator called" << endofline;
    if (this != &other) {
        // _name = other._name;
        _grade = other._grade;
    }
    return (*this);
}

Bureaucrat::~Bureaucrat() {
    std::cout << BOLD_RED << "Bureaucrat Destructor called" << endofline;
}

// METHODS
void Bureaucrat::gradeIncrease() {
    if (_grade <= 1)
        throw GradeTooHighException();
    _grade--;
}

void Bureaucrat::gradeDecrease() {
    if (_grade >= 150)
        throw GradeTooLowException();
    _grade++;
}

// EXCEPTIONS
const char* Bureaucrat::GradeTooHighException::what() const throw() {
    return "Bureaucrat: grade too high";
}

const char* Bureaucrat::GradeTooLowException::what() const throw() {
    return "Bureaucrat: grade too low";
}

// GETTERS
int         Bureaucrat::getGrade() const { return (_grade); }
std::string Bureaucrat::getName() const { return (_name);}

// FREE
std::ostream &operator<<(std::ostream &out, const Bureaucrat &bureaucrat) {
    out << bureaucrat.getName() << ", bureaucrat grade " << bureaucrat.getGrade();
    return (out);
}