#include "Zombie.hpp"

void	randomChump(std::string name) {
	Zombie Chump;
	Chump.F_SetName(name);
	Chump.announce();
}