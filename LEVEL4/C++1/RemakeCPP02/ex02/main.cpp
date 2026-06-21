#include "Fixed.hpp"
#include "utils.hpp"
#include <iostream>

int main(void) {
    std::cout << BOLD_CYAN << "=== Constructors & conversions ===" << RESET << std::endl;
    Fixed a;            // default -> 0
    Fixed b(42);        // int     -> 42
    Fixed c(3.14f);     // float   -> ~3.14
    Fixed d(c);         // copy of c
    Fixed e;
    e = b;              // copy-assign from b

    std::cout << "a (default) : " << a << "  [expect 0]" << std::endl;
    std::cout << "b (int 42)  : " << b << "  [expect 42]" << std::endl;
    std::cout << "c (3.14f)   : " << c << "  [expect ~3.14]" << std::endl;
    std::cout << "d (copy c)  : " << d << "  [expect ~3.14]" << std::endl;
    std::cout << "e (= b)     : " << e << "  [expect 42]" << std::endl;

    std::cout << std::endl << BOLD_CYAN << "=== getRawBits / setRawBits ===" << RESET << std::endl;
    std::cout << "c.getRawBits() : " << c.getRawBits() << "  [expect 804 = round(3.14*256)]" << std::endl;
    a.setRawBits(256);
    std::cout << "a after setRawBits(256) : " << a << "  [expect 1]" << std::endl;

    std::cout << std::endl << BOLD_CYAN << "=== toInt / toFloat ===" << RESET << std::endl;
    std::cout << "c.toInt()   : " << c.toInt()   << "  [expect 3]" << std::endl;
    std::cout << "c.toFloat() : " << c.toFloat() << "  [expect ~3.14]" << std::endl;

    std::cout << std::endl << BOLD_CYAN << "=== Comparison operators ===" << RESET << std::endl;
    Fixed two(2);
    Fixed three(3);
    std::cout << "2 <  3 : " << (two <  three) << "  [expect 1]" << std::endl;
    std::cout << "2 <= 2 : " << (two <= Fixed(2)) << "  [expect 1]" << std::endl;
    std::cout << "2 >  3 : " << (two >  three) << "  [expect 0]" << std::endl;
    std::cout << "3 >= 2 : " << (three >= two) << "  [expect 1]" << std::endl;
    std::cout << "2 == 2 : " << (two == Fixed(2)) << "  [expect 1]" << std::endl;
    std::cout << "2 != 3 : " << (two != three) << "  [expect 1]" << std::endl;

    std::cout << std::endl << BOLD_CYAN << "=== Arithmetic operators ===" << RESET << std::endl;
    Fixed x(10);
    Fixed y(3);
    std::cout << "10 + 3 : " << (x + y) << "  [expect 13]" << std::endl;
    std::cout << "10 - 3 : " << (x - y) << "  [expect 7]" << std::endl;
    std::cout << "10 * 3 : " << (x * y) << "  [expect 30]" << std::endl;
    std::cout << "10 / 3 : " << (x / y) << "  [expect ~3.33]" << std::endl;

    std::cout << std::endl << BOLD_CYAN << "=== Pre / post increment & decrement ===" << RESET << std::endl;
    Fixed inc;
    std::cout << "inc          : " << inc   << "  [expect 0]" << std::endl;
    std::cout << "++inc        : " << ++inc << "  [expect ~0.00390625]" << std::endl;
    std::cout << "inc          : " << inc   << "  [expect ~0.00390625]" << std::endl;
    std::cout << "inc++ returns: " << inc++ << "  [expect ~0.00390625 (old value)]" << std::endl;
    std::cout << "inc          : " << inc   << "  [expect ~0.0078125]" << std::endl;
    std::cout << "--inc        : " << --inc << "  [expect ~0.00390625]" << std::endl;
    std::cout << "inc-- returns: " << inc-- << "  [expect ~0.00390625 (old value)]" << std::endl;
    std::cout << "inc          : " << inc   << "  [expect 0]" << std::endl;

    std::cout << std::endl << BOLD_CYAN << "=== Static min / max ===" << RESET << std::endl;
    Fixed lo(1);
    Fixed hi(9);
    std::cout << "min(1, 9) (non-const) : " << Fixed::min(lo, hi) << "  [expect 1]" << std::endl;
    std::cout << "max(1, 9) (non-const) : " << Fixed::max(lo, hi) << "  [expect 9]" << std::endl;

    Fixed const clo(4);
    Fixed const chi(7);
    std::cout << "min(4, 7) (const)     : " << Fixed::min(clo, chi) << "  [expect 4]" << std::endl;
    std::cout << "max(4, 7) (const)     : " << Fixed::max(clo, chi) << "  [expect 7]" << std::endl;

    std::cout << std::endl << BOLD_CYAN << "=== Subject sample ===" << RESET << std::endl;
    Fixed s;
    Fixed const sb(Fixed(5.05f) * Fixed(2));
    std::cout << s << std::endl;
    std::cout << ++s << std::endl;
    std::cout << s << std::endl;
    std::cout << s++ << std::endl;
    std::cout << s << std::endl;
    std::cout << sb << std::endl;
    std::cout << Fixed::max(s, sb) << "  [expect ~10.1]" << std::endl;

    return 0;
}
