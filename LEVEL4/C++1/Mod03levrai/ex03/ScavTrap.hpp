#ifndef SCAVTRAP_HPP
# define SCAVTRAP_HPP

#include <string>
#include "ClapTrap.hpp"

class ScavTrap : public virtual ClapTrap {
    private:
        bool    _protect;

    protected:
        static const int _base_hp = 100;
        static const int _base_energy = 50;
        static const int _base_atk = 20;

    public:
        ScavTrap(std::string name);
        ScavTrap(const ScavTrap &src);
        ScavTrap& operator= (const ScavTrap &other);
        ~ScavTrap();

        void attack(const std::string& target);
        void guardGate();
};

#endif
