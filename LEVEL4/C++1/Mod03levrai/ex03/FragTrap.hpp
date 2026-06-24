#ifndef FRAGTRAP_HPP
# define FRAGTRAP_HPP

#include <string>
#include "ClapTrap.hpp"

class FragTrap : public virtual ClapTrap {
    protected :
        static const int _base_hp = 100;
        static const int _base_energy = 100;
        static const int _base_atk = 30;

    public:
        FragTrap(std::string name);
        FragTrap(const FragTrap &src);
        FragTrap& operator= (const FragTrap &other);
        ~FragTrap();

        void    highFivesGuys(void);
};

#endif
