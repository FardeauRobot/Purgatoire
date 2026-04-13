#include <iostream>
#include "Zombie.hpp"

int main () {
	Zombie *test = newZombie("Tim");
	test->announce();
	delete test;
	randomChump("Gneeee");
	randomChump("Gnaaaaa");
}
