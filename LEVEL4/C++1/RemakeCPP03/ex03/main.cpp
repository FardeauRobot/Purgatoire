#include "ClapTrap.hpp"
#include "ScavTrap.hpp"
#include "FragTrap.hpp"
#include "DiamondTrap.hpp"

int main(void) {
    // ClapTrap a("La Vie");
    // ScavTrap test1("Le Futur");
    // FragTrap coucou1("Le Passé");
    DiamondTrap testdiam1("Le Gueux");
    DiamondTrap testdiam2;
    DiamondTrap testdiam3 = testdiam1;


    // coucou1.highFivesGuys();
    testdiam1.attack("Fardeau");
    testdiam1.beRepaired(5999);
    testdiam1.whoAmI();
    testdiam2.whoAmI();
    testdiam3.whoAmI();
    testdiam1.attack("Fardeau");
    testdiam2.attack("Fardeau");
    testdiam3.attack("Fardeau");
    // for (int i = 0; i < 11; i++)
    // {
    //     test2.guardGate();
    //     test2.beRepaired(1000);
    //     test2.takeDamage(1000);
    //     test2.attack("Fardeau");
    //     a.takeDamage(1000);
    //     b.attack("Fardeau");
    //     e.beRepaired(8000);
    // }
    return 0;
}
