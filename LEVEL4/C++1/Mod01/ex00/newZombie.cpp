#include "Zombie.hpp"
#include <cstdlib>

Zombie* newZombie( std::string name ) {
	Zombie *O_NewZombie = new Zombie(name);
	return (O_NewZombie);
}