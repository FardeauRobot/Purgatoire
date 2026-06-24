#ifndef WRONGANIMAL_HPP
# define WRONGANIMAL_HPP

#include <string>

class WrongAnimal {
    protected:
        std::string type;

    public:
        WrongAnimal(void);
        WrongAnimal(std::string animal_type);
        WrongAnimal(const WrongAnimal &src);
        WrongAnimal& operator= (const WrongAnimal &other);
        virtual ~WrongAnimal();

        std::string getType(void) const;
        void makeSound(void) const;
};

#endif
