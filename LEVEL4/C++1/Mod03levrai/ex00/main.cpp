#include "ClapTrap.hpp"
#include <iostream>

int main(void) {
    ClapTrap a("Number 1");
    ClapTrap b("Number 2");
    ClapTrap c(a);

    std::string target = "Fardeau";

    a.attack(target);
    b.attack(target);
    c.attack(target);

    int testloop = a.getEnergy();
    for (int i = 0; i < testloop + 2; i++) {
        a.attack(target);
        // a.beRepaired(10);
        // a.takeDamage(5);

        // b.attack(target);
        // b.beRepaired(10);
        // b.takeDamage(5);

        // c.attack(target);
        // c.beRepaired(10);
        // c.takeDamage(5);
    }

    return 0;
}
