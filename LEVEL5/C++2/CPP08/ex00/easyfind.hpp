#ifndef EASYFIND_HPP
# define EASYFIND_HPP

#include <algorithm>
#include <exception>

class NoElemException : public std::exception {
    public :
        virtual const char *what() const throw ();
};

template<typename T>
typename T::iterator easyfind(T &container, int n) {
    typename T::iterator it;

    it = std::find(container.begin(), container.end(), n);
    if (it == container.end())
        throw NoElemException();
    return (it);
}

#endif
