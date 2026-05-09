#include "Animal.hpp"
#include "Dog.hpp"
#include "Cat.hpp"
#include "WrongAnimal.hpp"
#include "WrongCat.hpp"
#include <iostream>

int main(void) {
    // Animal a("ex00");
    // Animal b(a);

    // Dog D;
    // Cat C;

    // D.makeSound();
    // C.makeSound();
    // std::cout << "TYPE OF D = " << D.getType() << std::endl;
    // return 0;

    // const Animal* meta = new Animal();
    // const Animal* j = new Dog();
    // const Animal* i = new Cat();
    // std::cout << j->getType() << " " << std::endl;
    // std::cout << i->getType() << " " << std::endl;
    // i->makeSound(); //will output the cat sound!
    // j->makeSound();
    // meta->makeSound();
    // delete meta;
    // delete i;
    // delete j;

    WrongCat testa;

    testa.wrongSound();
}