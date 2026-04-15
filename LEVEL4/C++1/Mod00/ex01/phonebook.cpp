#include <iostream>
#include "PhoneBook.hpp"
#include "Contact.hpp"

int main (void)
{
	PhoneBook 		O_PhoneBook;
	std::string		S_Cmd;

	while (1)
	{
		std::cout << IN_COMMAND;
		if (!std::getline(std::cin, S_Cmd) || std::cin.eof())
			break;
		std::cout << std::endl;
		if (S_Cmd == "EXIT")
			break;
		if (S_Cmd == "ADD")
		{
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
			std::cout << "Filled all the phonebook !\n" << std::endl;
		}
		else
			std::cout << ERR_CMD << S_Cmd << "\n" << std::endl;
	}
	std::cout << "\nSEE YA !";
}