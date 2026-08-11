#ifndef ROBOTOMYREQUESTFORM_HPP
# define ROBOTOMYREQUESTFORM_HPP

#include <string>
#include "AForm.hpp"

class Bureaucrat;

class RobotomyRequestForm : public AForm {
    private:
        const std::string   _target;

        virtual void        executeAction() const;

    public:

        // ~TORS
        RobotomyRequestForm();
        RobotomyRequestForm(std::string target);
        RobotomyRequestForm(const RobotomyRequestForm &src);
        RobotomyRequestForm& operator= (const RobotomyRequestForm &other);
        virtual ~RobotomyRequestForm();

        // GETTERS / SETTERS
        std::string getTarget() const;
};

#endif
