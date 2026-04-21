#include "Harl.hpp"
#include "colors.hpp"
#include <iostream>


Harl::Harl() {
    std::cout << BOLD_GREEN << "Harl is apppearing" << std::endl;
}

Harl::~Harl() {
    std::cout << BOLD_RED << "Harl killed" << std::endl;
}

void Harl::debug(void) {
    std::cout << CYAN << MSG_DEBUG <<  RESET << std::endl;
}

void Harl::info(void) {
    std::cout << GREEN << MSG_INFO << RESET << std::endl;
}

void Harl::warning(void) {
    std::cout << YELLOW << MSG_WARNING << RESET << std::endl;
}

void Harl::error(void) {
    std::cout << BOLD_RED << MSG_ERROR << RESET <<  std::endl;
}

void Harl::complain(std::string level) {

    void (Harl::*functions[4])(void) = {
        &Harl::debug,
        &Harl::info,
        &Harl::warning,
        &Harl::error,
    };

    std::string arr_command[4] = {
        "DEBUG", "INFO", "WARNING", "ERROR"
    };

    int i;
    for (i = 0; i < 4; i++)
    {
        if (level == arr_command[i])
            (this->*functions[i])();
    }
}