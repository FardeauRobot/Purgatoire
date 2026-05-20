#include <iostream>
#include "utils.hpp"
#include "Weapon.hpp"
#include "HumanA.hpp"
#include "HumanB.hpp"

int main() {
    {
        Weapon club = Weapon("crude spiked club");
        Weapon club2 = Weapon("crude spiked club");
        HumanA bob("Bob", club);
        HumanA bor("Bor", club2);
        bob.attack();
        bor.attack();
        club.setType("some other type of club");
        club2.setType("some other type of club");
        bob.attack();
        bor.attack();
        std::cout << endofline;
    }
    std::cout << endofline;
    {
        Weapon club = Weapon("crude spiked club");
        HumanB jim("Jim");
        jim.setWeapon(club);
        jim.attack();
        club.setType("some other type of club");
        jim.attack();
        std::cout << endofline;
    }
    return 0;
}