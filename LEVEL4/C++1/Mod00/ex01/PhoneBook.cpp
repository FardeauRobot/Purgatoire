#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "PhoneBook.hpp"
#include "Contact.hpp"

PhoneBook::PhoneBook() : m_index(0), m_stored(0) {}

PhoneBook::PhoneBook(const PhoneBook& other) : m_index(other.m_index)
{
	for (int i = 0; i < 8; i++)
		m_Contact[i] = other.m_Contact[i];
}

PhoneBook& PhoneBook::operator=(const PhoneBook& other) {
	if (this != &other)
	{
		m_index = other.m_index;
		for (int i = 0; i < 8; i++)
			m_Contact[i]= other.m_Contact[i];
	}
	return (*this);
}

PhoneBook::~PhoneBook() {}

int		PhoneBook::F_Add(void) {
			Contact *O_Contact =  &m_Contact[m_index];
			m_index = ((m_index + 1) % 8);
			if (O_Contact->F_SetupContact(m_index) != SUCCESS)
				return (1);
			if (m_stored < 8)
			m_stored++;
			return (0);
}

int			PhoneBook::F_GetIndex(void) const {
	return (m_index);
}

void		PhoneBook::F_BookSetIndex (int newindex) {
	m_index = newindex;
}

Contact 	*PhoneBook::F_GetContact(int index) { 
	return (&m_Contact[index]); 
}
void	PhoneBook::F_BookPrint() {
	for (int i = 0; i < m_stored; i++)
		m_Contact[i].F_PrintContact();
}

void	PhoneBook::F_AddStored(void) {
	if (m_stored < 8)
		m_stored++;
}

void PhoneBook::F_FillAll(void) {
	static const char *first[]  = {"Alice", "Bob", "Charlie", "Diana", "Eve", "Frank", "Grace", "Hank"};
	static const char *last[]   = {"Smith", "Jones", "Brown", "White", "Black", "Green", "Hall", "King"};
	static const char *nick[]   = {"ali", "bobby", "chuck", "dee", "evie", "franky", "gracie", "hanky"};
	static const char *phone[]  = {"0611111111", "0622222222", "0633333333", "0644444444",
	                                "0655555555", "0666666666", "0677777777", "0688888888"};
	static const char *secret[] = {"hates cats", "afraid of dark", "snores loud", "eats paper",
	                                "talks to plants", "collects spoons", "sleeps standing", "scared of birds"};

	for (int i = 0; i < 8; i++)
	{
		m_Contact[i].F_FillContact(first[i], last[i], nick[i], phone[i], secret[i], i);
	}
	m_index = 0;
	m_stored = 8;
}

int	PhoneBook::F_Search(void) {
	std::string user_input;
	int converted_input;

	while (1)
	{
		std::cout << "Waiting for user index: ";
		if (!(std::cin >> user_input))
			return (FAILURE);
		converted_input = atoi(user_input.c_str());
		if (user_input == "EXIT")
		{
			std::cout << "Returning to main menu\n";
			break;
		}
		if (converted_input >= 1 && converted_input <= m_stored)
		{
			std::cout << std::setw(18) << std::left << "Chosen index: " << converted_input << std::endl;
			m_Contact[converted_input - 1].F_PrintSearch();
			break;
		}
		else
			std::cout << "No user existing at index: " << user_input << std::endl;
	}
	return (SUCCESS);
}
