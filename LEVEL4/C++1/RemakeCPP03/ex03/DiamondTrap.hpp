#ifndef DIAMONDTRAP_HPP
# define DIAMONDTRAP_HPP

#include <string>
#include "FragTrap.hpp"
#include "ScavTrap.hpp"

/*

    • Name, which is passed as a parameter to a constructor
    • ClapTrap::name (parameter of the constructor + "_clap_name" suffix)
    • Hit points (FragTrap)
    • Energy points (ScavTrap)
    • Attack damage (FragTrap)
    • attack() (ScavTrap)

*/

class DiamondTrap : public ScavTrap, public FragTrap{
    private:
        std::string m_name;

    public:
        DiamondTrap(void);
        DiamondTrap(std::string name);
        DiamondTrap(const DiamondTrap &src);
        DiamondTrap& operator= (const DiamondTrap &other);
        ~DiamondTrap();

        void attack(const std::string &target);
        void whoAmI();
};

#endif
