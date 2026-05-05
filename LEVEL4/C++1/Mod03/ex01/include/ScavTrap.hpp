#ifndef SCAVTRAP_HPP
# define SCAVTRAP_HPP

#include "ClapTrap.hpp"

class ScavTrap : public ClapTrap {
    private:
        bool m_gatekeep;

    public:
        ScavTrap(std::string name);
        ScavTrap(const ScavTrap &src);
        ScavTrap& operator= (const ScavTrap &other);
        ~ScavTrap();

        void guardGate();
};

#endif