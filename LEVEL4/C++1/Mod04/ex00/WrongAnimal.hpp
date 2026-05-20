#ifndef WRONGANIMAL_HPP
# define WRONGANIMAL_HPP

#include <string>

class WrongAnimal {
    private:
        std::string m_name;

    public:
        WrongAnimal(void);
        WrongAnimal(std::string name);
        WrongAnimal(const WrongAnimal &src);
        WrongAnimal& operator= (const WrongAnimal &other);
        ~WrongAnimal();

        virtual void wrongSound();
};

#endif
