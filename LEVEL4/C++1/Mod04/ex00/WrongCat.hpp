#ifndef WRONGCAT_HPP
# define WRONGCAT_HPP

#include <string>
#include "WrongAnimal.hpp"

class WrongCat : public WrongAnimal  {
    private:
        std::string m_name;

    public:
        WrongCat(void);
        WrongCat(std::string name);
        WrongCat(const WrongCat &src);
        WrongCat& operator= (const WrongCat &other);
        ~WrongCat();

};

#endif
