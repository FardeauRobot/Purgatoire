#include "utils.hpp"
#include "Weapon.hpp"
#include "HumanA.hpp"

HumanA::HumanA(std::string name, Weapon &to_equip) : name(name), equipped(to_equip){
    std::cout << BOLD_YELLOW << "Default constructor called\n" << endofline;
}

HumanA::~HumanA() {
    std::cout << BOLD_MAGENTA << name << "'s destructor has been called" << endofline;
}

void    HumanA::attack(void) {
    std::cout << BOLD_GREEN << name << RESET << " attack with their " << BOLD_RED << equipped.getType() << endofline;
}


