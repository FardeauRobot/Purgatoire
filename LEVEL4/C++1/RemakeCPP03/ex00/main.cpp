#include "ClapTrap.hpp"

int main(void) {
    ClapTrap e;
    ClapTrap a("default");
    ClapTrap b("La vie");

    for (int i = 0; i < 11; i++)
    {
        a.takeDamage(1000);
        b.attack("Fardeau");
        e.beRepaired(8000);
    }
    return 0;
}
