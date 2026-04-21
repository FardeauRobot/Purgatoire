#include <iostream>
#include "Zombie.hpp"

int main () {
	Zombie *test = newZombie("Tim");
	if (test)
	{
		test->announce();
		delete test;
	}
	randomChump("Gneeee");
	randomChump("Gnaaaaa");
}
