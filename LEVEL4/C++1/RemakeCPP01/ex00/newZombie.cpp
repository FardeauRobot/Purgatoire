#include "Zombie.hpp"

Zombie*	newZombie(std::string name) {
	Zombie* O_Zombie = new Zombie(name);
	return (O_Zombie);
}