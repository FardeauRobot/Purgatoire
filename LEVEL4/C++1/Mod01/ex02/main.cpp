#include <iostream>

int main (void) {

	std::string str = "HI THIS IS BRAIN";
	std::string *stringPTR = &str;
	std::string &stringREF = str;

	std::cout << "[PRINT ADDRESSES]\n" << std::endl;

	std::cout << "STR [" << &str << "]" << std::endl;
	std::cout << "PTR [" << stringPTR << "]" << std::endl;
	std::cout << "REF [" << &stringREF << "]" << std::endl;

	std::cout << "\n[PRINT VALUES]\n" << std::endl;
	std::cout << "STR [" << str << "]" << std::endl;
	std::cout << "PTR [" << *stringPTR << "]" << std::endl;
	std::cout << "REF [" << stringREF << "]" << std::endl;

}