#ifndef DIAMONDTRAP_HPP
# define DIAMONDTRAP_HPP

#include <string>
#include "ScavTrap.hpp"
#include "FragTrap.hpp"

class DiamondTrap : public ScavTrap, public FragTrap {
    private:
        std::string _name;

    public:
        DiamondTrap(std::string name);
        DiamondTrap(const DiamondTrap &src);
        DiamondTrap& operator= (const DiamondTrap &other);
        ~DiamondTrap();

        void attack(const std::string& target);
        void whoAmI();
        void    printStats(void) ;
};

#endif
