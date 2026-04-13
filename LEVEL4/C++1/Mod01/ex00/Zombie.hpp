#ifndef ZOMBIE_HPP
# define ZOMBIE_HPP

#include <string>

class Zombie
{
	private:
		std::string m_name;
	public:
		Zombie(void) { };
		Zombie(std::string name) ;
		~Zombie() ;

		void	F_SetName(std::string name);
		void announce(void);
};

Zombie*	newZombie(std::string name);
void	randomChump(std::string name);

#endif