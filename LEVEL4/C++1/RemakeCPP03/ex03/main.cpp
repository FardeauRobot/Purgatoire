#include "ClapTrap.hpp"
#include "ScavTrap.hpp"
#include "FragTrap.hpp"
#include "DiamondTrap.hpp"
#include <iostream>

int main(void) {
    std::cout << "\n=== DiamondTrap construction + OCF ===" << std::endl;
    DiamondTrap d("Diam");
    DiamondTrap copy(d);
    DiamondTrap assign("Temp");
    assign = d;

    std::cout << "\n=== whoAmI ===" << std::endl;
    d.whoAmI();

    std::cout << "\n=== Normal actions (hp=100 energy=50 atk=30) ===" << std::endl;
    d.attack("Target");     // DiamondTrap -> ScavTrap::attack  (CYAN)
    d.takeDamage(40);       // ScavTrap::takeDamage             (CYAN)
    d.beRepaired(20);       // ScavTrap::beRepaired             (CYAN)
    d.guardGate();          // ScavTrap::guardGate              (CYAN)
    d.guardGate();
    d.highFivesGuys();      // FragTrap::highFivesGuys          (BLUE)

    std::cout << "\n=== HP = 0: attack blocked, takeDamage shows 'already dead' ===" << std::endl;
    DiamondTrap dead("Doomed");
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
