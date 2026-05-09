#ifndef DOG_HPP
# define DOG_HPP

#include <string>
#include "Animal.hpp"

class Dog : virtual public Animal {
    private: 

    public:
        Dog(void);
        Dog(const Dog &src);
        Dog& operator= (const Dog &other);
        ~Dog();

        void makeSound() const;
};

#endif
