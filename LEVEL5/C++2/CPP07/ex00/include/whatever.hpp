#ifndef WHATEVER_HPP
# define WHATEVER_HPP

#include <string>

class whatever {
    private:
        std::string _name;

    public:
        whatever(std::string name);
        whatever(const whatever &src);
        whatever& operator= (const whatever &other);
        ~whatever();
};

#endif
