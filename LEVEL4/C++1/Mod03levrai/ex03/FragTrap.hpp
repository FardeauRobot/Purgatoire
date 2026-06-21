#ifndef FRAGTRAP_HPP
# define FRAGTRAP_HPP

#include <string>
#include "ClapTrap.hpp"

class FragTrap : public virtual ClapTrap {

    public:
        FragTrap(std::string name);
        FragTrap(const FragTrap &src);
        FragTrap& operator= (const FragTrap &other);
        ~FragTrap();

        void    highFivesGuys(void);
};

#endif
