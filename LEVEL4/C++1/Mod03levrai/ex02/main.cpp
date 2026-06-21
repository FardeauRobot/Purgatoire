#include "ClapTrap.hpp"
#include "FragTrap.hpp"
#include "ScavTrap.hpp"
#include <iostream>

int main(void) {
    // ClapTrap a("Number 1");
    // ScavTrap b("Number 2");
    // ClapTrap c(a);

    std::string target = "Fardeau";

    // a.attack(target);
    // b.attack(target);
    // c.attack(target);
    // b.guardGate();

    // ScavTrap d(b);
    // d.guardGate();

    FragTrap e("Number 3");
    e.highFivesGuys();
    // int testloop = b.getEnergy();
    for (int i = 0; i < 5; i++) {
        // a.attack(target);
        // a.beRepaired(10);
        // a.takeDamage(5);

        // b.attack(target);
        // b.beRepaired(10);
        // b.takeDamage(5);

        // c.attack(target);
        // c.beRepaired(10);
        // c.takeDamage(5);

        // d.attack(target);
        // d.beRepaired(10);
        // d.takeDamage(5);

        e.attack(target);
        e.beRepaired(10);
        e.takeDamage(5);
    }

    return 0;
}