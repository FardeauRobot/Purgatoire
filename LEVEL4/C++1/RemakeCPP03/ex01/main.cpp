#include "ClapTrap.hpp"
#include "ScavTrap.hpp"
#include <iostream>

int main(void) {
    std::cout << "\n=== ScavTrap construction + OCF ===" << std::endl;
    ScavTrap s("Scout");
    ScavTrap copy(s);
    ScavTrap assign("Temp");
    assign = s;

    std::cout << "\n=== ScavTrap normal actions (hp=100 energy=50 atk=20) ===" << std::endl;
    s.guardGate();          // start guarding
    s.guardGate();          // already guarding
    s.attack("Target");     // energy: 50 -> 49
    s.takeDamage(60);       // hp: 100 -> 40
    s.beRepaired(10);       // hp: 40 -> 50, energy: 49 -> 48

    std::cout << "\n=== HP = 0: attack blocked, takeDamage shows 'already dead' ===" << std::endl;
    ScavTrap dead("Doomed");
    dead.takeDamage(100);
    dead.attack("Target");
    dead.takeDamage(1);

    std::cout << "\n=== Energy = 0: attack and repair blocked (ClapTrap, 10 energy) ===" << std::endl;
    ClapTrap c("Clappy");
    for (int i = 0; i < 10; i++)
        c.attack("Target");
    c.attack("Target");
    c.beRepaired(10);

    return 0;
}
