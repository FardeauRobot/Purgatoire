#ifndef ZOMBIE_HPP
# define ZOMBIE_HPP

#include <string>

class Zombie {
    private:
        std::string m_name;

    public:
        Zombie(void);
        Zombie(std::string name);
        Zombie(const Zombie &src);
        Zombie& operator= (const Zombie &other);
        ~Zombie();

        void announce(void);
        void F_SetName(std::string name);
        std::string F_GetName(void) const;
};

Zombie*	newZombie(std::string name);
void	randomChump(std::string name);

#endif
