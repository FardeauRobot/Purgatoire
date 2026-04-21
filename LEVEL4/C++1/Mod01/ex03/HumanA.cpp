#include "HumanA.hpp"

HumanA::HumanA(const std::string& name, Weapon& weapon)
    : name(name), weapon(weapon) {
}

HumanA::HumanA(const HumanA& other)
    : name(other.name), weapon(other.weapon) {
}

HumanA& HumanA::operator=(const HumanA& other) {
    if (this != &other)
        name = other.name;
    return *this;
}

HumanA::~HumanA() {
}

void HumanA::attack() const {
    std::cout << name << " attacks with their " << weapon.getType() << std::endl;
}
