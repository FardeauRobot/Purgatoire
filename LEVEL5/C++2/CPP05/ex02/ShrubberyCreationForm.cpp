#include <fstream>
#include <iostream>
#include "ShrubberyCreationForm.hpp"
#include "Bureaucrat.hpp"
#include "utils.hpp"

ShrubberyCreationForm::ShrubberyCreationForm()
    : AForm("ShrubberyCreationForm", 145, 137), _target("Default") {
    std::cout << BOLD_MAGENTA << "ShrubberyCreationForm Default constructor called" << endofline;
}

ShrubberyCreationForm::ShrubberyCreationForm(std::string target)
    : AForm("ShrubberyCreationForm", 145, 137), _target(target) {
    std::cout << BOLD_CYAN << "ShrubberyCreationForm Target constructor called" << endofline;
}

ShrubberyCreationForm::ShrubberyCreationForm(const ShrubberyCreationForm &src)
    : AForm(src), _target(src._target) {
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

std::string ShrubberyCreationForm::getTarget() const { return (_target);}

void    ShrubberyCreationForm::executeAction() const {
    std::string filepath = _target + "_shrubbery";
    std::ofstream ofs(filepath.c_str());
    if (!ofs) {
        std::cerr << BOLD_RED << "cannot open " << filepath << endofline;
        return ;
    }
    ofs << "           _-_" << std::endl
        << "        /~~   ~~\\" << std::endl
        << "     /~~         ~~\\" << std::endl
        << "    {               }" << std::endl
        << "     \\  _-     -_  /" << std::endl
        << "      ~  \\\\   //  ~" << std::endl
        << "     _-    | |    -_" << std::endl
        << "      _ -  | |  - _" << std::endl
        << "          // \\\\" << std::endl;
    
}
