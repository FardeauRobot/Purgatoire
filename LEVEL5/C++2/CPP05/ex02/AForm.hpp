#ifndef AFORM_HPP
# define AFORM_HPP

#include <string>
#include <exception>
#include <iostream>

class Bureaucrat;

class AForm {
    protected:
        const std::string   _name;
        bool                _signed;
        const int           _gradeToSign;
        const int           _gradeToExecute;

        virtual void        executeAction() const = 0;

    public:

        // ~TORS
        AForm();
        AForm(std::string name, int gradeToSign, int gradeToExecute);
        AForm(const AForm &src);
        AForm& operator= (const AForm &other);
        virtual ~AForm();

        // GETTERS / SETTERS
        std::string getName() const;
        bool        getSigned() const;
        int         getGradeToSign() const;
        int         getGradeToExecute() const;

        // METHODS
        void            beSigned(const Bureaucrat &bureaucrat);
        void            execute(const Bureaucrat &executor) const;

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
        class FormNotSignedException : public std::exception {
            private:
                std::string _msg;

            public:
                FormNotSignedException(const std::string &msg);
                virtual ~FormNotSignedException() throw() ;
                virtual const char * what () const throw ();
        };
};

std::ostream& operator<< (std::ostream& os, AForm const& form);

#endif
