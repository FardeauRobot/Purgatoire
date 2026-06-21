#include "ClapTrap.hpp"
#include "ScavTrap.hpp"
#include "FragTrap.hpp"
#include <iostream>

int main(void) {
    std::cout << "\n=== FragTrap construction + OCF ===" << std::endl;
    FragTrap f("Fragger");
    FragTrap copy(f);
    FragTrap assign("Temp");
    assign = f;

    std::cout << "\n=== FragTrap normal actions (hp=100 energy=100 atk=30) ===" << std::endl;
    f.highFivesGuys();      // FragTrap-specific
    f.attack("Target");     // energy: 100 -> 99  (BLUE: FragTrap::attack)
    f.takeDamage(50);       // hp: 100 -> 50      (GREEN: ClapTrap::takeDamage, not overridden)
    f.beRepaired(20);       // hp: 50 -> 70       (GREEN: ClapTrap::beRepaired, not overridden)

    std::cout << "\n=== HP = 0: attack blocked, takeDamage shows 'already dead' ===" << std::endl;
    FragTrap dead("Doomed");
    dead.takeDamage(100);
    dead.attack("Target");
    dead.beRepaired(50);    // ClapTrap::beRepaired checks hp too: blocked
    dead.takeDamage(1);

    std::cout << "\n=== Energy = 0: attack and repair blocked (ClapTrap, 10 energy) ===" << std::endl;
    ClapTrap c("Clappy");
    for (int i = 0; i < 10; i++)
        c.attack("Target");
    c.attack("Target");
    c.beRepaired(10);

    return 0;
}
