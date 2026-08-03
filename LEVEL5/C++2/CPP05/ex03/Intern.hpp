#ifndef INTERN_HPP
# define INTERN_HPP

#include <string>

#include "AForm.hpp"

class Intern {
    public:
        Intern();
        Intern(const Intern &src);
        Intern& operator= (const Intern &other);
        ~Intern();

        // METHODS
        AForm *makeForm(std::string name, std::string target) const;
};

#endif
