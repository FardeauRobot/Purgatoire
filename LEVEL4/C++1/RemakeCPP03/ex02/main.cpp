#include "ClapTrap.hpp"
#include "ScavTrap.hpp"
#include "FragTrap.hpp"

int main(void) {
    ClapTrap a("La Vie");
    ScavTrap test1("Le Futur");
    FragTrap coucou1("Le Passé");

    coucou1.highFivesGuys();

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
