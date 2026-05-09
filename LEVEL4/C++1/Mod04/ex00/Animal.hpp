#ifndef ANIMAL_HPP
# define ANIMAL_HPP

#include <string>

class Animal {
    protected:
        std::string type;

    public:
        Animal(void);
        Animal(std::string type);
        Animal(const Animal &src);
        Animal& operator= (const Animal &other);
        virtual ~Animal();

        virtual void makeSound() const;
        virtual std::string getType() const;
};

#endif
