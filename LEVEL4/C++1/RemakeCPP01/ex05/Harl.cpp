#include <iostream>
#include "Harl.hpp"
#include "utils.hpp"

Harl::Harl(void) {
    std::cout << BOLD_CYAN << "Harl Default constructor called" << RESET << std::endl;
}

Harl::Harl(std::string name): m_name(name) {
    std::cout << BOLD_CYAN << "Harl Name constructor called" << RESET << std::endl;
}

Harl::Harl(const Harl &src) {
    std::cout << BOLD_BLUE << "Harl Copy constructor called" << RESET << std::endl;
    *this = src;
}

Harl& Harl::operator= (const Harl &other) {
    std::cout << BOLD_BLUE << "Harl Copy assignment operator called" << RESET << std::endl;
    if (this != &other)
        m_name = other.m_name;
    return (*this);
}

Harl::~Harl() {
    std::cout << BOLD_RED << "Harl Destructor called" << RESET << std::endl;
}

void Harl::debug(void) {
    std::cout << BOLD_CYAN << "Here's the debugging part" << endofline;
}

void Harl::info(void) {
    std::cout << BOLD_WHITE << "Here's the info part" << endofline;
}

void Harl::warning(void) {
    std::cout << BOLD_YELLOW << "Here's the warning part" << endofline;
}

void Harl::error(void) {
    std::cout << BOLD_RED << "Here's the error part" << endofline;
}

void Harl::complain(std::string level) {
    std::string Str_Level[4] = {"DEBUG", "INFO", "WARNING", "ERROR"};

    HarlFunctions functions_table[4] = { &Harl::debug, &Harl::info, &Harl::warning, &Harl::error};

    for (int i = 0; i < 4; i++)
    {
        if (level == Str_Level[i])
            (this->*functions_table[i])();
    }
}