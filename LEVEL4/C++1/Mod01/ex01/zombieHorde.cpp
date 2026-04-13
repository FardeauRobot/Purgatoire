#include "Zombie.hpp"
#include <cstdlib>
#include <iostream>

Zombie* zombieHorde( int N, std::string name ) {
	Zombie *O_Horde = new Zombie[N] ;
	for (int i = 0; i < N; i++)
	{
		std::cout << O_Horde[i].F_GetName() << " is evolving !\n";
		std::cout << O_Horde[i].F_GetName() << " is now named: ";
		O_Horde[i].F_SetName(name);
		std::cout << O_Horde[i].F_GetName() << std::endl;
	}
	return (O_Horde);
}