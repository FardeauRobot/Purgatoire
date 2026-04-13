#include "Zombie.hpp"

void randomChump( std::string name ) {
	Zombie O_NewZombie(name);
	O_NewZombie.announce();
}