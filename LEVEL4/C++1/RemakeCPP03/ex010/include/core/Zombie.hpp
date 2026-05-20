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
};

#endif
