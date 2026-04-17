#include "HumanB.hpp"

HumanB::HumanB(const std::string& name)
    : name(name), weapon(NULL) {
}

HumanB::HumanB(const HumanB& other)
    : name(other.name), weapon(other.weapon) {
}

HumanB& HumanB::operator=(const HumanB& other) {
    if (this != &other) {
        name = other.name;
        weapon = other.weapon;
    }
    return *this;
}

HumanB::~HumanB() {
}

void HumanB::setWeapon(Weapon& weapon) {
    this->weapon = &weapon;
}

void HumanB::attack() const {
    if (weapon == NULL)
        std::cout << name << " has no weapon to attack with" << std::endl;
    else
        std::cout << name << " attacks with their " << weapon->getType() << std::endl;
}
