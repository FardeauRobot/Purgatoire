#include "Zombie.hpp"
#include "utils.hpp"

int main(void) {
    try 
    {
        Zombie a("Foo");
        Zombie b(a);

        a.announce();
        b.announce();

        Zombie *p_horde = zombieHorde(10, "Test");
        delete[] p_horde;
    }
    catch (std::exception &e)
    {
        std::cout << BOLD_RED << "Error while running program ..." << endofline;
    }
    return 0;
}
