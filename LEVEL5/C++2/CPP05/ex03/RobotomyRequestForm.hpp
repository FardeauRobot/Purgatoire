#ifndef ROBOTOMYREQUESTFORM_HPP
# define ROBOTOMYREQUESTFORM_HPP

#include <string>

#include "AForm.hpp"

class RobotomyRequestForm : public AForm {
    private:
        const std::string _target;

    protected:
        virtual void executeAction() const;

    public:
        RobotomyRequestForm();
        RobotomyRequestForm(std::string target);
        RobotomyRequestForm(const RobotomyRequestForm &src);
        RobotomyRequestForm& operator= (const RobotomyRequestForm &other);
        virtual ~RobotomyRequestForm();

        // GETTERS
        std::string getTarget() const;
};

#endif
