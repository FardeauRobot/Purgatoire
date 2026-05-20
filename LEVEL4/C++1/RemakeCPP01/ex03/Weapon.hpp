#ifndef WEAPON_HPP
# define WEAPON_HPP

#include <string>

class Weapon {
    private:
        std::string type;

    public:
        Weapon(void);
        Weapon(std::string name);
        Weapon(const Weapon &src);
        Weapon& operator= (const Weapon &other);
        ~Weapon();

        const std::string &getType(void) ;
        void    setType(std::string change_to);
        void    printType(void);
};

#endif
