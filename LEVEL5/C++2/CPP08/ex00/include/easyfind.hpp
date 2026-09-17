#ifndef EASYFIND_HPP
# define EASYFIND_HPP

#include <string>

class easyfind {
    private:
        std::string _name;

    public:
        easyfind(std::string name);
        easyfind(const easyfind &src);
        easyfind& operator= (const easyfind &other);
        ~easyfind();
};

#endif
