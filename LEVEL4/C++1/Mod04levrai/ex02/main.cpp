#include <cstddef>
#include <iostream>
#include <new>

#include "utils.hpp"
#include "Animal.hpp"
#include "Brain.hpp"
#include "Cat.hpp"
#include "Dog.hpp"

#define SHELTER_SIZE 20
int main() {
	// TESTS SUR CLASSES -- only compile when Animal is concrete (ABSTRACT == 0)
#if ABSTRACT == 0
	const Animal *meta = NULL;
#endif
	const Animal *cat = NULL;
	const Animal *dog = NULL;

	Animal* animal_shelter[SHELTER_SIZE];
	for (int i = 0; i < SHELTER_SIZE; i++)
		animal_shelter[i] = NULL;

	try {

#if ABSTRACT == 0
		meta = new Animal();
#endif
		cat = new Cat();
		dog = new Dog();

#if ABSTRACT == 0
		std::cout << "Animal type = " << meta->getType() << std::endl;
#endif
		std::cout << "Animal type = " << cat->getType() << std::endl;
		std::cout << "Animal type = " << dog->getType() << std::endl;

#if ABSTRACT == 0
		meta->makeSound();
#endif
		cat->makeSound();
		dog->makeSound();

		// TESTS DES BRAINS
		std::cout << "FIRST DOG" << endofline;
		Cat test_cat;
		Dog test_dog;
		Brain *brain_test = test_dog.getBrain();
		brain_test->fillBrain();
		brain_test->printBrain();

		std::cout << "SECOND DOG" << endofline;
		Dog test_dog2(test_dog);
		brain_test = test_dog2.getBrain();
		brain_test->fillBrain();
		brain_test->printBrain();
		test_dog2.makeSound();


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

		for (int i = 0; i < SHELTER_SIZE / 2; i++) {
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

#if ABSTRACT == 0
	delete (meta);
#endif
	delete (cat);
	delete (dog);

	for (int i = 0; i < SHELTER_SIZE; i++)
		delete (animal_shelter[i]);
	return 0;
}
