#include <cstddef>
#include <iostream>
#include <new>
// #include <stdexcept>
// #include <string>

#include "utils.hpp"
#include "Animal.hpp"
#include "Brain.hpp"
#include "Cat.hpp"
#include "Dog.hpp"
// #include "WrongAnimal.hpp"
// #include "WrongCat.hpp"

#define SHELTER_SIZE 20
int main() {
	// const Animal *meta = NULL;
	// const Animal *cat  = NULL;
	// const Animal *dog  = NULL;

	Animal* animal_shelter[SHELTER_SIZE];
	for (int i = 0; i < SHELTER_SIZE; i++)
		animal_shelter[i] = NULL;

	try {
		// TESTS SUR CLASSES 
		// meta = new Animal();
		// cat  = new Cat();
		// dog  = new Dog();

		// std::cout << "Animal type = " << meta->getType() << std::endl;
		// std::cout << "Animal type = " << cat->getType() << std::endl;
		// std::cout << "Animal type = " << dog->getType() << std::endl;

		// meta->makeSound();
		// cat->makeSound();
		// dog->makeSound();

		// TESTS DES BRAINS
		// std::cout << "FIRST DOG" << endofline;
		// Cat test_cat;
		// Dog test_dog;
		// Brain *brain_test = test_dog.getBrain();
		// brain_test->fillBrain();
		// brain_test->printBrain();

		// std::cout << "SECOND DOG" << endofline;
		// Dog test_dog2(test_dog);
		// brain_test = test_dog2.getBrain();
		// brain_test->fillBrain();
		// brain_test->printBrain();
		// test_dog2.makeSound();

		// TESTS ARRAY ANIMALS
		for (int i = 0; i < SHELTER_SIZE; i++) {
			if (i < 10) {
				animal_shelter[i] = new Dog;
			}
			else
				animal_shelter[i] = new Cat;
			animal_shelter[i]->makeSound();
		}

		Dog* dog_og = static_cast<Dog *>(animal_shelter[0]);
		dog_og->getBrain()->fillBrain();

		Cat* cat_og = reinterpret_cast<Cat *>(animal_shelter[SHELTER_SIZE / 2]);
		cat_og->getBrain()->fillBrain();

		for (int i = 1; i < SHELTER_SIZE / 2; i++) {
			*static_cast<Dog*>(animal_shelter[i]) = *dog_og;
			if (i % 3 == 0) {
				std::cout << BOLD_CYAN <<"BRAIN OF DOGGO NUMBER [" << i << "]" << endofline;
				static_cast<Dog*>(animal_shelter[i])->getBrain()->printBrain();
			}
		}

		for (int i = 10; i < SHELTER_SIZE; i++) {
			*static_cast<Cat*>(animal_shelter[i]) = *cat_og;
			if (i % 3 == 0) {
				std::cout << BOLD_CYAN <<"BRAIN OF CATS NUMBER [" << i << "]\n" << endofline;
				static_cast<Cat*>(animal_shelter[i])->getBrain()->printBrain();
			}
		}

	} catch (std::bad_alloc &e) {
		std::cerr << BOLD_YELLOW << "[BAD ALLOC] " << e.what() << endofline;
	}

	// delete meta;
	// delete cat;
	// delete dog;

	for (int i = 0; i < SHELTER_SIZE; i++)
		delete (animal_shelter[i]);
	return 0;
}
