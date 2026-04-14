#include <iostream>

#include "Weapon.hpp"
#include "HumanA.hpp"
#include "HumanB.hpp"

int main (void) {
		Weapon test("Dudule");
		test.setType("Gogole");
		std::cout << test.getType() << std::endl;
}