#include <cstddef>
#include <iostream>
#include <new>

#include "Animal.hpp"
#include "Cat.hpp"
#include "Dog.hpp"
#include "WrongAnimal.hpp"
#include "WrongCat.hpp"
#include "utils.hpp"

int main() {
	const Animal      *meta       = NULL;
	const Animal      *cat          = NULL;
	const Animal      *dog          = NULL;
	const WrongAnimal *wrong_meta = NULL;
	const WrongAnimal *wrong_cat  = NULL;
	WrongCat wrong_cat2;

	try {
		meta       = new Animal();
		cat          = new Cat();
		dog          = new Dog();
		wrong_meta = new WrongAnimal();
		wrong_cat  = new WrongCat();

		std::cout << "Animal type = " << meta->getType() << std::endl;
		std::cout << "Animal type = " << cat->getType() << std::endl;
		std::cout << "Animal type = " << dog->getType() << std::endl;
		std::cout << "Animal type = " << wrong_cat->getType() << std::endl;
		std::cout << "Animal type = " << wrong_meta->getType() << std::endl;
		std::cout << "Animal type = " << wrong_cat2.getType() << std::endl;

		meta->makeSound();
		cat->makeSound();
		dog->makeSound();
		wrong_meta->makeSound();
		wrong_cat->makeSound();
		wrong_cat2.makeSound();

	} catch (std::bad_alloc &e) {
		std::cerr << BOLD_YELLOW << "[ERROR ALLOC] " << e.what() << endofline;
	}

	delete meta;
	delete cat;
	delete dog;
	delete wrong_meta;
	delete wrong_cat;
	return 0;
}
