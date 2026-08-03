#ifndef SHRUBBERYCREATIONFORM_HPP
# define SHRUBBERYCREATIONFORM_HPP

#include <string>

#include "AForm.hpp"

class ShrubberyCreationForm : public AForm {
    private:
        const std::string _target;

    protected:
        virtual void executeAction() const;

    public:
        ShrubberyCreationForm();
        ShrubberyCreationForm(std::string target);
        ShrubberyCreationForm(const ShrubberyCreationForm &src);
        ShrubberyCreationForm& operator= (const ShrubberyCreationForm &other);
        virtual ~ShrubberyCreationForm();

        // GETTERS
        std::string getTarget() const;
};

#endif
