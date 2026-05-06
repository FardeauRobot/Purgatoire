#ifndef CLAPTRAP_HPP
# define CLAPTRAP_HPP

#include <string>

class ClapTrap {
	protected :
		std::string		m_name;
		unsigned int				m_hp;
		unsigned int				m_energy;
		unsigned int				m_atk;

	public:
		ClapTrap(void);
		ClapTrap(std::string name);
		ClapTrap(const ClapTrap &src);
		ClapTrap& operator= (const ClapTrap &other);
		~ClapTrap();

		void attack(const std::string &target);
		void beRepaired(unsigned int amount);
		void takeDamage(unsigned int amount);
};

#endif
