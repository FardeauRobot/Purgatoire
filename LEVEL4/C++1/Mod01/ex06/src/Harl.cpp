#include "Harl.hpp"
#include "colors.hpp"
#include <iostream>


Harl::Harl() {
    // std::cout << GREEN << "Harl is apppearing" << RESET <<  std::endl;
}

Harl::~Harl() {
    // std::cout << RED << "Harl killed" << RESET << std::endl;
}

void Harl::debug(void) {
    std::cout << BOLD_WHITE << "[DEBUG]" << RESET << std::endl;
    std::cout << BOLD_CYAN << MSG_DEBUG <<  RESET << std::endl;
}

void Harl::info(void) {
    std::cout << BOLD_WHITE << "[INFO]" << RESET << std::endl;
    std::cout << BOLD_GREEN << MSG_INFO << RESET << std::endl;
}

void Harl::warning(void) {
    std::cout << BOLD_WHITE << "[WARNING]" << RESET << std::endl;
    std::cout << BOLD_YELLOW << MSG_WARNING << RESET << std::endl;
}

void Harl::error(void) {
    std::cout << BOLD_WHITE << "[ERROR]" << RESET << std::endl;
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
            break;
    }

    switch (i)
    {
        case 0:
            (this->*functions[0])();
            // fall through
        case 1:
            (this->*functions[1])();
            // fall through
        case 2:
            (this->*functions[2])();
            // fall through
        case 3:
            (this->*functions[3])();
            // fall through
            break;
        default:
            std::cout << BOLD_MAGENTA << DEFAULT_MSG << RESET << std::endl;
            // fall through
    }
}