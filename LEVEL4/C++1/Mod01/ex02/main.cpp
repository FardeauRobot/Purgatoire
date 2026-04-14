#include <iostream>

#include "Weapon.hpp"
#include "HumanA.hpp"
#include "HumanB.hpp"

int main (void) {
	std::string str = "HI THIS IS BRAIN";
	std::string *stringPTR = &str;
	std::string &stringREF = str;

	std::cout << "STR = " << &str << std::endl;
	std::cout << "PTR = " << stringPTR << std::endl;
	std::cout << "REF = " << &stringREF << std::endl;
}