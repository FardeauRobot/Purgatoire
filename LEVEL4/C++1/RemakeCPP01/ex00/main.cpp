#include "Zombie.hpp"

int main(void) {
    Zombie a("Foo");
    Zombie b(a);

    a.announce();
    b.announce();

    Zombie *p_zombie = newZombie("Test Heap");
    p_zombie->announce();


    randomChump("Test le 2");

    delete p_zombie;
    return 0;
}
