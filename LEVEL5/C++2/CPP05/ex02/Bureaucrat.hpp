#ifndef BUREAUCRAT_HPP
# define BUREAUCRAT_HPP

#include <exception>
#include <iostream>
#include <string>

class AForm;

class Bureaucrat {
    private:
        const std::string _name;
        int         _grade;

    public:
        Bureaucrat();
        Bureaucrat(std::string name, int grade);
        Bureaucrat(const Bureaucrat &src);
        Bureaucrat& operator= (const Bureaucrat &other);
        ~Bureaucrat();

        // METHODS
        void gradeIncrease();
        void gradeDecrease();
        void signForm(AForm &form) const;
        void executeForm(const AForm &form) const;

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
        int getGrade() const;
        std::string getName() const;
};

// FREE
std::ostream &operator<<(std::ostream &out, const Bureaucrat &bureaucrat);

#endif
