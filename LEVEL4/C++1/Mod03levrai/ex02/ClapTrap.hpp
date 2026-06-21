#ifndef CLAPTRAP_HPP
# define CLAPTRAP_HPP

#include <string>

class ClapTrap {
    protected:
        std::string     _name;
        int             _hp;         
        int             _energy;         
        int             _atk;         

    public:

        ClapTrap(std::string name);
        ClapTrap(const ClapTrap &src);
        ClapTrap& operator= (const ClapTrap &other);
        ~ClapTrap();

        void    attack(const std::string& target);
        void    takeDamage(unsigned int amount);
        void    beRepaired(unsigned int amount);

        int     getEnergy(void);
};

#endif
