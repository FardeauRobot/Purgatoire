#ifndef BUREAUCRAT_HPP
# define BUREAUCRAT_HPP

#include <string>
#include <exception>
#include <iostream>

class Form;

class Bureaucrat {
    private:
        const std::string _name;
        int _grade;

    public:

        // ~TORS
        Bureaucrat();
        Bureaucrat(std::string name, int grade);
        Bureaucrat(const Bureaucrat &src);
        Bureaucrat& operator= (const Bureaucrat &other);
        ~Bureaucrat();

        // GETTERS / SETTERS
        std::string getName() const;
        int getGrade() const;

        // METHODS
        void    incrementGrade();
        void    decrementGrade();
        void    signForm(Form& form);

        
        // EXCEPTIONS
        class GradeTooHighException : public std::exception {
            private: 
                std::string _msg;

            public:
                GradeTooHighException(const std::string &name);
                virtual ~GradeTooHighException() throw() ;
                virtual const char * what () const throw ();
        };
        class GradeTooLowException : public std::exception {
            private: 
                std::string _msg;

            public:
                GradeTooLowException(const std::string &name);
                virtual ~GradeTooLowException() throw() ;
                virtual const char * what () const throw ();
        };
};

std::ostream& operator<< (std::ostream& os, Bureaucrat const& bureaucrat);

#endif
