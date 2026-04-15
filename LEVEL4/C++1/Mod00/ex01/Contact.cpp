#include <iostream>
#include <iomanip>
#include "PhoneBook.hpp"
#include "Contact.hpp"

Contact::Contact() : m_first_name(""), m_last_name(""), m_nickname(""), m_darkest_secret(""), m_index(0) {}

Contact::Contact(const Contact& other)
	: m_first_name(other.m_first_name), 
	m_last_name(other.m_last_name),
	m_nickname(other.m_last_name),
	m_darkest_secret(other.m_darkest_secret),
	m_index(other.m_index) {}

Contact& Contact::operator=(const Contact& other) {
	if (this != &other) {
		m_first_name = other.m_first_name;
		m_last_name = other.m_last_name;
		m_nickname = other.m_nickname;
		m_index = other.m_index;
		m_darkest_secret = other.m_darkest_secret;
	}
	return (*this);
}
Contact::~Contact() {}

void	Contact::F_ContactSetIndex(int index) {
	m_index = index;
}

static std::string F_PrintString(std::string str) {
	if (str.length() > 10)
		return (str.substr(0, 9) + ".");
	return (str);
}

static int	F_PromptField(std::string& field, const std::string& label) {
	do {
		std::cout << std::setw(WIDTH_SETUP) << label;
		if (!std::getline(std::cin, field) || std::cin.eof())
			return (FAILURE);
		if (field.empty())
			std::cout << IN_EMPTY << std::endl;
	} while (field.empty());
	return (SUCCESS);
}

int	Contact::F_SetupContact(int index) {

	std::cout.flags(std::cout.left);

	if (F_PromptField(m_first_name, IN_FIRSTNAME) == FAILURE)
		return (FAILURE);
	if (F_PromptField(m_last_name, IN_LASTNAME) == FAILURE)
		return (FAILURE);
	if (F_PromptField(m_nickname, IN_NICKNAME) == FAILURE)
		return (FAILURE);
	if (F_PromptField(m_phone_number, IN_PHONE) == FAILURE)
		return (FAILURE);
	if (F_PromptField(m_darkest_secret, IN_SECRET) == FAILURE)
		return (FAILURE);

	m_index = index;
	std:: cout << REGISTERED << std::endl;
	return (SUCCESS);
}

void	Contact::F_FillContact(std::string first_name, std::string last_name, std::string nickname, std::string phone_number, std::string secret, int index) {
	m_first_name = first_name;	
	m_last_name = last_name;	
	m_nickname = nickname;
	m_phone_number = phone_number;
	m_darkest_secret = secret;
	m_index = index + 1;
}

void	Contact::F_PrintSearch(void) {
	std::cout.flags(std::cout.left);

	std::cout	<< std::setw(WIDTH_CHOSEN) << "First name: " << m_first_name << std::endl
				<< std::setw(WIDTH_CHOSEN) << "Last name: " << m_last_name << std::endl
				<< std::setw(WIDTH_CHOSEN) << "Nickname: " << m_nickname << std::endl
				<< std::setw(WIDTH_CHOSEN) << "Phone Number: " << m_phone_number << std::endl
				<< std::setw(WIDTH_CHOSEN) << "Darkest secret: " << m_darkest_secret << "\n" << std::endl;
}


void	Contact::F_PrintContact(void) {
	std::cout.flags(std::cout.right);
	char c = '|';

	std::cout	<< c << std::setw(WIDTH_INDEX) << m_index 
				<< c << std::setw(WIDTH_INDEX) << F_PrintString(m_first_name)
				<< c << std::setw(WIDTH_INDEX) << F_PrintString(m_last_name)
				<< c << std::setw(WIDTH_INDEX) << F_PrintString(m_nickname)
				<< c << std::endl;
}