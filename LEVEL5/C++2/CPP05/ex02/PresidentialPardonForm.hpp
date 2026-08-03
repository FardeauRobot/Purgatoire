#ifndef PRESIDENTIALPARDONFORM_HPP
# define PRESIDENTIALPARDONFORM_HPP

#include <string>

#include "AForm.hpp"

class PresidentialPardonForm : public AForm {
    private:
        const std::string _target;

    protected:
        virtual void executeAction() const;

    public:
        PresidentialPardonForm();
        PresidentialPardonForm(std::string target);
        PresidentialPardonForm(const PresidentialPardonForm &src);
        PresidentialPardonForm& operator= (const PresidentialPardonForm &other);
        virtual ~PresidentialPardonForm();

        // GETTERS
        std::string getTarget() const;
};

#endif
