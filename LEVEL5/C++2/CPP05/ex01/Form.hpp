#ifndef FORM_HPP
# define FORM_HPP

#include <string>
#include <exception>
#include <iostream>

class Bureaucrat;

class Form {
    private:
        const std::string   _name;
        bool                _signed;
        const int           _gradeToSign;
        const int           _gradeToExecute;

    public:

        // ~TORS
        Form();
        Form(std::string name, int gradeToSign, int gradeToExecute);
        Form(const Form &src);
        Form& operator= (const Form &other);
        ~Form();

        // GETTERS / SETTERS
        std::string getName() const;
        bool        getSigned() const;
        int         getGradeToSign() const;
        int         getGradeToExecute() const;

        // METHODS
        void    beSigned(const Bureaucrat &bureaucrat);

        // EXCEPTIONS
        class GradeTooHighException : public std::exception {
            private:
                std::string _msg;

            public:
                GradeTooHighException(const std::string &msg);
                virtual ~GradeTooHighException() throw() ;
                virtual const char * what () const throw ();
        };
        class GradeTooLowException : public std::exception {
            private:
                std::string _msg;

            public:
                GradeTooLowException(const std::string &msg);
                virtual ~GradeTooLowException() throw() ;
                virtual const char * what () const throw ();
        };
};

std::ostream& operator<< (std::ostream& os, Form const& form);

#endif
