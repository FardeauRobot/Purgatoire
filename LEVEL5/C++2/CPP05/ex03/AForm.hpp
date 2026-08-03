#ifndef AFORM_HPP
# define AFORM_HPP

#include <exception>
#include <iostream>
#include <string>

#include "Bureaucrat.hpp"

class AForm {
    private:
        const std::string _name;
        bool        _signed;
        const int   _signGrade;
        const int   _execGrade;

    protected:
        // The actual work of a concrete form. Called by execute() only once
        // the signature and grade requirements have been verified.
        virtual void executeAction() const = 0;

    public:
        AForm();
        AForm(std::string name, int signGrade, int execGrade);
        AForm(const AForm &src);
        AForm& operator= (const AForm &other);
        virtual ~AForm();

        // METHODS
        void beSigned(const Bureaucrat &bureaucrat);
        void execute(const Bureaucrat &executor) const;

        // EXCEPTIONS
        class GradeTooHighException : public std::exception {
            public:
                const char *what() const throw();
        };

        class GradeTooLowException : public std::exception {
            public:
                const char *what() const throw();
        };

        class FormNotSignedException : public std::exception {
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
std::ostream &operator<<(std::ostream &out, const AForm &form);

#endif
