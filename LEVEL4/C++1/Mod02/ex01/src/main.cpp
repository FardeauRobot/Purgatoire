#include <iostream>
#include <stdexcept>
#include "Fixed.hpp"
#include "Errors.hpp"

/*
    Default constructor called
    Int constructor called
    Float constructor called
    Copy constructor called
    Copy assignment operator called

    Float constructor called
    Copy assignment operator called
    Destructor called

    a is 1234.43
    b is 10
    c is 42.4219
    d is 10

    a is 1234 as integer
    b is 10 as integer
    c is 42 as integer
    d is 10 as integer

    Destructor called
    Destructor called
    Destructor called
    Destructor called
*/
int main(int argc, char **argv)
{
    try
    {
        (void)argv;
        if (argc != 1)
            F_ErrMsg("Invalid number of args");
        Fixed a;
        Fixed const b( 10 );
        Fixed const c( 42.42f );
        Fixed const d( b );

        a = Fixed( 1234.4321f );

        std::cout << "a is " << a << std::endl;
        std::cout << "b is " << b << std::endl;
        std::cout << "c is " << c << std::endl;
        std::cout << "d is " << d << std::endl;

        std::cout << "a is " << a.toInt() << " as integer" << std::endl;
        std::cout << "b is " << b.toInt() << " as integer" << std::endl;
        std::cout << "c is " << c.toInt() << " as integer" << std::endl;
        std::cout << "d is " << d.toInt() << " as integer" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (FAILURE);
    }
    return (SUCCESS);
}
