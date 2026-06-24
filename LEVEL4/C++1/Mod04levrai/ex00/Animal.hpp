#ifndef ANIMAL_HPP
# define ANIMAL_HPP

#include <string>

class Animal {
    protected:
        std::string type;

    public:
        Animal(void);
        Animal(std::string animal_type);
        Animal(const Animal &src);
        Animal& operator= (const Animal &other);
        virtual ~Animal();

        virtual std::string getType(void) const;
        virtual void makeSound(void) const;
};

#endif
