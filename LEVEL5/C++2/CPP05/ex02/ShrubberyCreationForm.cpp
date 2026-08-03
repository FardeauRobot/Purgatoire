#include <fstream>
#include <iostream>

#include "ShrubberyCreationForm.hpp"
#include "utils.hpp"

// ~TORS
ShrubberyCreationForm::ShrubberyCreationForm():
    AForm("ShrubberyCreationForm", 145, 137), _target("Default") {
    std::cout << BOLD_CYAN << "ShrubberyCreationForm Default constructor called" << endofline;
}

ShrubberyCreationForm::ShrubberyCreationForm(std::string target):
    AForm("ShrubberyCreationForm", 145, 137), _target(target) {
    std::cout << BOLD_CYAN << "ShrubberyCreationForm Target constructor called" << endofline;
}

ShrubberyCreationForm::ShrubberyCreationForm(const ShrubberyCreationForm &src):
    AForm(src), _target(src._target) {
    std::cout << BOLD_BLUE << "ShrubberyCreationForm Copy constructor called" << endofline;
}

ShrubberyCreationForm& ShrubberyCreationForm::operator= (const ShrubberyCreationForm &other) {
    std::cout << BOLD_BLUE << "ShrubberyCreationForm Copy assignment operator called" << endofline;
    if (this != &other)
        AForm::operator=(other);
    return (*this);
}

ShrubberyCreationForm::~ShrubberyCreationForm() {
    std::cout << BOLD_RED << "ShrubberyCreationForm Destructor called" << endofline;
}

// METHODS
void ShrubberyCreationForm::executeAction() const {
    std::ofstream out((_target + "_shrubbery").c_str());

    if (!out.is_open()) {
        std::cerr << BOLD_RED << "ShrubberyCreationForm: cannot open "
                  << _target << "_shrubbery" << endofline;
        return ;
    }
    out << "       ###\n"
        << "      #o###\n"
        << "    #####o###\n"
        << "   #o#\\#|#/###\n"
        << "    ###\\|/#o#\n"
        << "     # }|{  #\n"
        << "       }|{\n";
    out.close();
}

// GETTERS
std::string ShrubberyCreationForm::getTarget() const { return (_target); }
