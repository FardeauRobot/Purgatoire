#include <iostream>
#include <stdexcept>
#include "ClapTrap.hpp"
#include "ScavTrap.hpp"
#include "errors.hpp"
#include "colors.hpp"

int main(int argc, char **argv)
{
    try
    {
        (void)argv;
        (void)argc;

        ScavTrap minion("Jean");
        ScavTrap minion2(minion);

        ScavTrap minion3 = minion2;

        minion3.guardGate();
        minion3.guardGate();
        for (int i = 0; i < 26; i++)
        {
            minion3.attack("Fardeau");
            minion2.attack("Fardeau");
            minion3.beRepaired(10);
            minion2.beRepaired(10);
        }
        // TESTING
        // for (int i = 0; i < 11; i++)
        // {
            // minion.takeDamage(1);
            // minion.beRepaired(10);
            // minion.attack("Fardeau");
        // }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (FAILURE);
    }
    return (SUCCESS);
}
