#ifndef HUMANA_HPP
# define HUMANA_HPP

class HumanA {
    private:
        std::string name;
        Weapon &equipped;

    public: 
        HumanA(std::string name, Weapon &to_equp);
        ~HumanA();

        void    attack(void);
};
#endif 