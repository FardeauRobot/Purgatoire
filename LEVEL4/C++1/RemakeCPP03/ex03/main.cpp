#include <iostream>
#include "ClapTrap.hpp"
#include "ScavTrap.hpp"
#include "FragTrap.hpp"
#include "DiamondTrap.hpp"

int main(void) {
    DiamondTrap testdiam1("Le Gueux");
    std::cout << std::endl;

    std::cout << "ENERGY DIAM1 : " << testdiam1.getEnergy() << std::endl;

    DiamondTrap testdiam2;
    std::cout << std::endl;

    DiamondTrap testdiam3 = testdiam1;
    std::cout << std::endl;

    // coucou1.highFivesGuys();
    testdiam1.attack("Fardeau");
    // testdiam1.beRepaired(5999);
    testdiam1.whoAmI();
    std::cout << std::endl;

    testdiam2.attack("Fardeau");
    testdiam2.beRepaired(5999);
    testdiam2.whoAmI();
    std::cout << std::endl;

    testdiam3.attack("Fardeau");
    testdiam3.beRepaired(5999);
    testdiam3.whoAmI();

    std::cout << std::endl;

    return 0;
}
