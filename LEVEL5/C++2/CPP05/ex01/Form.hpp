#ifndef FORM_HPP
# define FORM_HPP

#include <exception>
#include <iostream>
#include <string>

#include "Bureaucrat.hpp"

class Form {
    private:
        const std::string _name;
        bool        _signed;
        const int   _signGrade;
        const int   _execGrade;

    public:
        Form();
        Form(std::string name, int signGrade, int execGrade);
        Form(const Form &src);
        Form& operator= (const Form &other);
        ~Form();

        // METHODS
        void beSigned(const Bureaucrat &bureaucrat);

        // EXCEPTIONS
        class GradeTooHighException : public std::exception {
            public:
                const char *what() const throw();
        };

        class GradeTooLowException : public std::exception {
            public:
                const char *what() const throw();
        };

        // GETTERS
        std::string getName() const;
        bool        getSigned() const;
        int         getSignGrade() const;
        int         getExecGrade() const;
};

// FREE
std::ostream &operator<<(std::ostream &out, const Form &form);

#endif
