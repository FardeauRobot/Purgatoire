#include "Fixed.hpp"
#include "utils.hpp"
#include <iostream>

int main(void) {
  Fixed a;
  Fixed const b(10);
  Fixed const c(42.42f);
  Fixed const d(b);

  a = Fixed(1234.4321f);

  std::cout << "a is " << a << std::endl;
  std::cout << "b is " << b << std::endl;
  std::cout << "c is " << c << std::endl;
  std::cout << "d is " << d << std::endl;

  std::cout << BOLD_YELLOW << "a is " << a.toInt() << " as integer" << RESET << std::endl;
  std::cout << BOLD_YELLOW << "b is " << b.toInt() << " as integer" << RESET << std::endl;
  std::cout << BOLD_YELLOW << "c is " << c.toInt() << " as integer" << RESET << std::endl;
  std::cout << BOLD_YELLOW << "d is " << d.toInt() << " as integer" << RESET << std::endl;
  return 0;
}