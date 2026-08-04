#ifndef BUREAUCRAT_HPP
# define BUREAUCRAT_HPP

#include <string>
#include <exception>
#include "utils.hpp"

class Bureaucrat {
    private:
        const std::string _name;
        int _grade;

    public:

        Bureaucrat(std::string name, int grade);
        Bureaucrat(const Bureaucrat &src);
        Bureaucrat& operator= (const Bureaucrat &other);
        ~Bureaucrat();

        class GradeTooHighException : public std::exception {
            private: 
                std::string _msg;

            public:
                GradeTooHighException(const std::string &name);
                virtual const char * what () const throw ();
        };

        class GradeTooLowException : public std::exception {
            private: 
                std::string _msg;

            public:
                GradeTooLowException(const std::string &name);
                virtual const char * what () const throw ();
        };
};

#endif
