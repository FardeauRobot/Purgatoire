#ifndef SHRUBBERYCREATIONFORM_HPP
# define SHRUBBERYCREATIONFORM_HPP

#include <string>
#include "AForm.hpp"

class Bureaucrat;

class ShrubberyCreationForm : public AForm {
    private:
        const std::string   _target;

        virtual void        executeAction() const;

    public:

        // ~TORS
        ShrubberyCreationForm();
        ShrubberyCreationForm(std::string target);
        ShrubberyCreationForm(const ShrubberyCreationForm &src);
        ShrubberyCreationForm& operator= (const ShrubberyCreationForm &other);
        virtual ~ShrubberyCreationForm();

        // GETTERS / SETTERS
        std::string getTarget() const;
};

#endif
