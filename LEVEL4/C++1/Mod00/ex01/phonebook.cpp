#include <iostream>
#include "PhoneBook.hpp"
#include "Contact.hpp"

int main (void)
{
	PhoneBook 		O_PhoneBook;
	Contact			*O_Contact;
	std::string		S_Cmd;

	while (S_Cmd != "EXIT")
	{
		std::cout << "Waiting for the next command :\n";
		std::cin >> S_Cmd;
		if (S_Cmd == "ADD")
		{
			int index = O_PhoneBook.F_GetIndex();
			O_Contact =  O_PhoneBook.F_GetContact(index);
			O_Contact->F_SetupContact();
			O_PhoneBook.F_BookSetIndex((index + 1) % 8);
			O_PhoneBook.F_AddStored();
		}
		else if (S_Cmd == "SEARCH")
			O_PhoneBook.F_BookPrint();
		else if (S_Cmd == "FILL")
			O_PhoneBook.F_FillAll();
	}
	std::cout << "SEE YA !\n";
}