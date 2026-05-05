#ifndef SCAVTRAP_HPP
# define SCAVTRAP_HPP

#include <string>
#include "ClapTrap.hpp"

class ScavTrap : public ClapTrap {
    protected:
        bool    m_guarding;

    public:
        ScavTrap(void);
        ScavTrap(std::string name);
        ScavTrap(const ScavTrap &src);
        ScavTrap& operator= (const ScavTrap &other);
        ~ScavTrap();

		void attack(const std::string &target);
		void beRepaired(unsigned int amount);
		void takeDamage(unsigned int amount);
        void guardGate();
};

#endif
