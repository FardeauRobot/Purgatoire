#ifndef ZOMBIE_HPP
# define ZOMBIE_HPP

#include <string>

class Zombie
{
	private:
		std::string m_name;

	public:
		Zombie(void);
		Zombie(std::string name);
		Zombie(const Zombie &src);
		~Zombie();

		Zombie &operator=(const Zombie &other);

		void	F_SetName(std::string name);
		std::string	F_GetName(void);
		void announce(void);
};

Zombie*	newZombie(std::string name);
Zombie* zombieHorde( int N, std::string name );

#endif