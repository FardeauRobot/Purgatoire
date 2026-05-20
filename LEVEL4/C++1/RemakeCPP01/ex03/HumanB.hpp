#ifndef HUMANB_HPP
# define HUMANB_HPP

class HumanB {
    private:
        std::string name;
        Weapon *equipped;

    public: 
        HumanB(std::string name);
        ~HumanB();

        void    attack(void);
        void    setWeapon(Weapon &to_equip);
};
#endif 