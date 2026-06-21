#include "ClapTrap.hpp"
#include <iostream>

int main(void) {
    std::cout << "\n=== Construction + OCF ===" << std::endl;
    ClapTrap a("Alpha");
    ClapTrap b(a);
    ClapTrap c("Temp");
    c = a;

    std::cout << "\n=== Normal actions (hp=10 energy=10 atk=0) ===" << std::endl;
    a.attack("Target");     // energy: 10 -> 9
    a.takeDamage(3);        // hp: 10 -> 7
    a.beRepaired(5);        // hp: 7 -> 12, energy: 9 -> 8

    std::cout << "\n=== Drain energy to 0 (8 actions left) ===" << std::endl;
    for (int i = 0; i < 8; i++)
        a.attack("Target");

    std::cout << "\n=== Energy = 0: attack and repair blocked ===" << std::endl;
    a.attack("Target");
    a.beRepaired(10);

    std::cout << "\n=== HP = 0: attack and repair blocked ===" << std::endl;
    ClapTrap dead("Doomed");
    dead.takeDamage(100);
    dead.attack("Target");
    dead.beRepaired(100);
    dead.takeDamage(1);

    return 0;
}
