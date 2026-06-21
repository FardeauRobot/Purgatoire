#include <iostream>
#include "ClapTrap.hpp"
#include "ScavTrap.hpp"
#include "FragTrap.hpp"
#include "DiamondTrap.hpp"

int main(void) {
    // ClapTrap a("Number 1");
    // ScavTrap b("Number 2");
    // ClapTrap c(a);
    // FragTrap e("Number 3");

    DiamondTrap f("Diamond");
    DiamondTrap g(f);

    std::string target = "Fardeau";

    // a.attack(target);
    // b.attack(target);
    // c.attack(target);
    // b.guardGate();

    // ScavTrap d(b);
    // d.guardGate();

    // e.highFivesGuys();

    f.whoAmI();
    f.guardGate();
    f.highFivesGuys();

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

        // e.attack(target);
        // e.beRepaired(10);
        // e.takeDamage(5);

        f.attack(target);
        f.beRepaired(10);
        f.takeDamage(5);

        g.attack(target);
        g.beRepaired(10);
        g.takeDamage(5);
    }

    return 0;
}
