#ifndef HARL_HPP
# define HARL_HPP

#include <string>

class Harl {
    private:
        std::string m_name;

        void debug(void);
        void info(void);
        void warning(void);
        void error(void);

    public:
        Harl(void);
        Harl(std::string name);
        Harl(const Harl &src);
        Harl& operator= (const Harl &other);
        ~Harl();

        void complain(std::string level);
};

typedef void (Harl::*HarlFunctions)();

#endif
