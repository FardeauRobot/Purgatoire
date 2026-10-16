#include "easyfind.hpp"

const char *NoElemException::what() const throw () {
    return ("No element found in the container.");
}
