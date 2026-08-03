#include "Bureaucrat.hpp"
#include "utils.hpp"
#include <iostream>

int main(void) {
	try {
#if TOO_HIGH == 1
		Bureaucrat a("Bureaucrat A", 0);
		std::cout << BOLD_YELLOW << a << endofline;
#endif

#if TOO_HIGH == 2
		Bureaucrat a("Bureaucrat A", 1);
		std::cout << BOLD_YELLOW << a << endofline;
		a.gradeIncrease();
#endif

#if TOO_LOW == 1
		Bureaucrat c("Bureaucrat C", 151);
		std::cout << BOLD_YELLOW << c << endofline;
#endif

#if TOO_LOW == 2
		Bureaucrat c("Bureaucrat C", 150);
		std::cout << BOLD_YELLOW << c << endofline;
		c.gradeDecrease();
#endif


		Bureaucrat b("Bureaucrat B", 1);
		std::cout << BOLD_YELLOW << b << endofline;

	} catch (std::exception &e) {
		std::cout << BOLD_RED << e.what() << endofline;
	}
	return 0;
}