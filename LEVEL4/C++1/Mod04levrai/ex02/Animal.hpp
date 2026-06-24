#ifndef ANIMAL_HPP
# define ANIMAL_HPP

#include <string>

#define ABSTRACT 1

class Animal {
    protected:
        std::string type;

    public:
        Animal(void);
        Animal(std::string animal_type);
        Animal(const Animal &src);
        Animal& operator= (const Animal &other);
        virtual ~Animal();

#if ABSTRACT == 1
        virtual void makeSound(void) const = 0;
#else
        virtual void makeSound(void) const;
#endif
        virtual std::string getType(void) const;
};

#endif
