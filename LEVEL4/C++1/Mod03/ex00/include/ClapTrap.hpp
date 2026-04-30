#ifndef CLAPTRAP_HPP
# define CLAPTRAP_HPP

#include <iostream>

class ClapTrap {
    private:
        std::string m_name;
        int m_hp = 10;
        int m_energy = 10;
        int m_atk = 0;

    public:
        ClapTrap(std::string name);
        ClapTrap(const ClapTrap &src);
        ClapTrap& operator= (const ClapTrap &other);
        ~ClapTrap();

        void attack(const std::string& target);
        void takeDamage(unsigned int amount);
        void beRepaired(unsigned int amount);
}

#endif