#include "Zombie.hpp"
#include <cstdlib>

Zombie* newZombie( std::string name ) {
	Zombie *O_NewZombie = new Zombie(name);
	if (!O_NewZombie)
		return (NULL);
	return (O_NewZombie);
}