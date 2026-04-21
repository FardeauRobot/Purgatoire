#include <iostream>
#include "Zombie.hpp"

int main (void) {
	std::string name = "Four Tet";

	Zombie *arr = zombieHorde(10, name);
	for (int i = 0; i < 10; i++)
	{
		arr[i].announce();
		std::cout << "Zombie number " << i + 1 << " lives at address: " << &arr[i] << std::endl;
	}
	delete[] arr;
}
