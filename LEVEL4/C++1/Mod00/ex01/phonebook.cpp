#include <iostream>
#include "PhoneBook.hpp"
#include "Contact.hpp"

int main (void)
{
	PhoneBook 		O_PhoneBook;
	std::string		S_Cmd;

	while (1)
	{
		std::cout << "Waiting for the next command : ";
		if (!(std::cin >> S_Cmd) || S_Cmd == "EXIT")
			break;
		std::cout << std::endl;
		if (S_Cmd == "ADD")
		{
			std::cout << std::endl;
			if (O_PhoneBook.F_Add() != SUCCESS)
				break;
		}
		else if (S_Cmd == "SEARCH")
		{
			O_PhoneBook.F_BookPrint();
			std::cout << std::endl;
			if (O_PhoneBook.F_Search() != SUCCESS)
				break;
		}
		else if (S_Cmd == "FILL")
		{
			O_PhoneBook.F_FillAll();
			std::cout << std::endl;
		}
		else
			std::cout << "Wrong command: " << S_Cmd << "\n\n";
	}
	std::cout << "SEE YA !\n";
}