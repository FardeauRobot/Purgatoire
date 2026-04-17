#ifndef HUMANA_HPP
# define HUMANA_HPP

#include <iostream>
#include <string>
#include "Weapon.hpp"

class HumanA {
    private:
        std::string name;
        Weapon&     weapon;

    public:
        HumanA(const std::string& name, Weapon& weapon);
        HumanA(const HumanA& other);
        HumanA& operator=(const HumanA& other);
        ~HumanA();

        void attack() const;
};

#endif
