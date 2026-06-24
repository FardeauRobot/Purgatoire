#ifndef WRONGCAT_HPP
# define WRONGCAT_HPP

#include "WrongAnimal.hpp"

class WrongCat : public WrongAnimal {

    public:
        WrongCat(void);
        WrongCat(std::string animal_type);
        WrongCat(const WrongCat &src);
        WrongCat& operator= (const WrongCat &other);
        ~WrongCat();

        void makeSound(void) const;
};

#endif
