#include "utils.hpp"
#include "Weapon.hpp"
#include "HumanB.hpp"

HumanB::HumanB(std::string name) : name(name), equipped(NULL) {
    std::cout << BOLD_YELLOW << name << " Default constructor called\n" << endofline;
}

HumanB::~HumanB() {
    std::cout << BOLD_MAGENTA << name << "'s destructor has been called" << endofline;
}

void    HumanB::setWeapon(Weapon &to_equip) {
    equipped = &to_equip;
}

void    HumanB::attack(void) {
    std::cout << BOLD_GREEN << name << RESET << " attack with their " << BOLD_RED << equipped->getType() << endofline;
}
