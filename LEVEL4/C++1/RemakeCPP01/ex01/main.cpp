#include "Zombie.hpp"
#include "utils.hpp"

int main(void) {
    try 
    {
        int nb_horde = 10;
        Zombie a("Foo");
        Zombie b(a);

        a.announce();
        b.announce();

        Zombie *p_horde = zombieHorde(nb_horde, "Test");
        for (int i = 0; i < nb_horde; i++)
            p_horde[i].announce();

        delete[] p_horde;
    }
    catch (std::exception &e)
    {
        std::cout << BOLD_RED << "Error while running program ..." << endofline;
    }
    return 0;
}
