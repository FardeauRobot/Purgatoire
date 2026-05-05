#include <iostream>
#include <stdexcept>
#include "Fixed.hpp"
#include "Errors.hpp"

int main(int argc, char **argv)
{
    try
    {
        (void)argv;
        if (argc < 1)
            return (F_ErrMsg("invalid invocation"));
        Fixed a;
        Fixed b( a );
        Fixed c;

        c = b;

        std::cout << a.getRawBits() << std::endl;
        std::cout << b.getRawBits() << std::endl;
        std::cout << c.getRawBits() << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (FAILURE);
    }
    return (SUCCESS);
}
