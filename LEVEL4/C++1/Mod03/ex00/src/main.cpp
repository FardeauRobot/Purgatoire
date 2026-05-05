#include <iostream>
#include <stdexcept>
#include "ClapTrap.hpp"
#include "errors.hpp"

int main(int argc, char **argv)
{
    try
    {
        (void)argv;

        ClapTrap minion("Jean");

        if (argc < 1)
            return (F_ErrMsg("invalid invocation"));

        // TESTING
        for (int i = 0; i < 11; i++)
        {
            minion.takeDamage(1);
            // minion.beRepaired(10);
            // minion.attack("Fardeau");
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (FAILURE);
    }
    return (SUCCESS);
}
