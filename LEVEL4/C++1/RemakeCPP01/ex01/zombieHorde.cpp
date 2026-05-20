#include "Zombie.hpp"

Zombie* zombieHorde(int N, std::string name) {
	Zombie* O_Horde = new Zombie[N];

	for (int i = 0; i < N; i++)
	{
		O_Horde[i].F_SetName(name);
	}
	return (O_Horde);
}