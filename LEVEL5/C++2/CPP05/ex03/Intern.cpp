#include <iostream>

#include "Intern.hpp"
#include "PresidentialPardonForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "ShrubberyCreationForm.hpp"
#include "utils.hpp"

namespace {
    AForm *makeShrubbery(std::string target) {
        return (new ShrubberyCreationForm(target));
    }

    AForm *makeRobotomy(std::string target) {
        return (new RobotomyRequestForm(target));
    }

    AForm *makePardon(std::string target) {
        return (new PresidentialPardonForm(target));
    }

    struct FormEntry {
        const char *name;
        AForm *(*build)(std::string target);
    };

    const FormEntry g_forms[] = {
        { "shrubbery creation", &makeShrubbery },
        { "robotomy request",   &makeRobotomy  },
        { "presidential pardon", &makePardon   }
    };

    const size_t g_formCount = sizeof(g_forms) / sizeof(g_forms[0]);
}

// ~TORS
Intern::Intern() {
    std::cout << BOLD_CYAN << "Intern Default constructor called" << endofline;
}

Intern::Intern(const Intern &src) {
    std::cout << BOLD_BLUE << "Intern Copy constructor called" << endofline;
    (void)src;
}

Intern& Intern::operator= (const Intern &other) {
    std::cout << BOLD_BLUE << "Intern Copy assignment operator called" << endofline;
    (void)other;
    return (*this);
}

Intern::~Intern() {
    std::cout << BOLD_RED << "Intern Destructor called" << endofline;
}

// METHODS
AForm *Intern::makeForm(std::string name, std::string target) const {
    for (size_t i = 0; i < g_formCount; i++) {
        if (name == g_forms[i].name) {
            std::cout << BOLD_GREEN << "Intern creates " << name << endofline;
            return (g_forms[i].build(target));
        }
    }
    std::cerr << BOLD_RED << "Intern: no form named \"" << name << "\"" << endofline;
    return (NULL);
}
