#ifndef PRESIDENTIALPARDONFORM_HPP
# define PRESIDENTIALPARDONFORM_HPP

#include <string>
#include "AForm.hpp"

class Bureaucrat;

class PresidentialPardonForm : public AForm {
    private:
        const std::string   _target;

        virtual void        executeAction() const;

    public:

        // ~TORS
        PresidentialPardonForm();
        PresidentialPardonForm(std::string target);
        PresidentialPardonForm(const PresidentialPardonForm &src);
        PresidentialPardonForm& operator= (const PresidentialPardonForm &other);
        virtual ~PresidentialPardonForm();

        // GETTERS / SETTERS
        std::string getTarget() const;
};

#endif
