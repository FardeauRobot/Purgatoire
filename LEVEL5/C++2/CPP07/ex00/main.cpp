#include <iostream>
#include <stdexcept>
#include "errors.hpp"
#include "utils.hpp"
#include "whatever.hpp"

#if SUBJECT == 0
int main(void)
{
    int a = 10;
    int b = 20;

    std::cout << BOLD_CYAN << "[A] = " << a << " || [B] = " << b << endofline;
    std::cout << BOLD_WHITE << "MIN = " << min(a, b) << endofline;
    std::cout << BOLD_YELLOW << "MAX = " << max(a, b) << endofline;
    swap(a, b);

    std::cout << BOLD_GREEN << "SWAP \n[A] = " << a << " || [B] = " << b  << endofline;

    return (SUCCESS);
}

#else
int main( void ) {
    int a = 2;
    int b = 3;

    ::swap( a, b );

    std::cout << "a = " << a << ", b = " << b << std::endl;
    std::cout << "min( a, b ) = " << ::min( a, b ) << std::endl;
    std::cout << "max( a, b ) = " << ::max( a, b ) << std::endl;

    std::string c = "chaine1";
    std::string d = "chaine2";

    ::swap(c, d);

    std::cout << "c = " << c << ", d = " << d << std::endl;
    std::cout << "min( c, d ) = " << ::min( c, d ) << std::endl;
    std::cout << "max( c, d ) = " << ::max( c, d ) << std::endl;
    return 0;
}
#endif
