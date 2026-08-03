#include <cstdlib>
#include <ctime>
#include <iostream>

#include "AForm.hpp"
#include "Bureaucrat.hpp"
#include "Intern.hpp"
#include "utils.hpp"

static void title(const std::string &text) {
    std::cout << BOLD_MAGENTA << "\n=== " << text << " ===" << endofline;
}

int main(void) {
    std::srand(static_cast<unsigned int>(std::time(NULL)));

    Intern someRandomIntern;

    title("The intern builds every known form");
    {
        const char *names[3] = {
            "shrubbery creation",
            "robotomy request",
            "presidential pardon"
        };
        Bureaucrat boss("Zaphod", 1);

        for (int i = 0; i < 3; i++) {
            AForm *form = someRandomIntern.makeForm(names[i], "Bender");

            if (form == NULL)
                continue ;
            std::cout << BOLD_YELLOW << *form << endofline;
            boss.signForm(*form);
            boss.executeForm(*form);
            delete form;
        }
    }

    title("An unknown form name is rejected");
    {
        AForm *form = someRandomIntern.makeForm("coffee making", "Milton");

        if (form == NULL)
            std::cout << BOLD_YELLOW << "makeForm returned NULL, nothing to delete" << endofline;
        delete form;
    }

    title("The subject's example");
    {
        AForm *rrf = someRandomIntern.makeForm("robotomy request", "Bender");

        if (rrf != NULL) {
            std::cout << BOLD_YELLOW << *rrf << endofline;
            delete rrf;
        }
    }

    return 0;
}
