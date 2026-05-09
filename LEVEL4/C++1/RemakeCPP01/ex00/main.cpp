#include "Zombie.hpp"

int main(void) {
    Zombie a("Foo");
    Zombie b(a);

    a.announce();
    b.announce();

    Zombie *p_zombie;
    p_zombie = newZombie("Test");
    p_zombie->announce();
    delete p_zombie;

    randomChump("Test le 2");

    return 0;
}
