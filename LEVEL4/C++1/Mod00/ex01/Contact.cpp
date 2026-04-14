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

int	Contact::F_SetupContact(int index) {
	std:: cout << "Insert first name :\n";
	if (!(std:: cin >> m_first_name))
		return (FAILURE);
	std:: cout << "Insert last name :\n";
	if (!(std:: cin >> m_last_name))
		return (FAILURE);
	std:: cout << "Insert nickname:\n";
	if (!(std:: cin >> m_nickname))
		return (FAILURE);
	std:: cout << "Insert phone number:\n";
	if (!(std:: cin >> m_phone_number))
		return (FAILURE);
	std:: cout << "What's your darkest secret ? >.<\n";
	if (!(std:: cin >> m_darkest_secret))
		return (FAILURE);
	m_index = index;
	std:: cout << "\nUser has been well registered!\n";
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
	std::cout	
				<< std::setw(18) << std::left << "First name: " << m_first_name << std::endl
				<< std::setw(18) << std::left << "Last name: " << m_last_name << std::endl
				<< std::setw(18) << std::left << "Nickname: " << m_nickname << std::endl
				<< std::setw(18) << std::left << "Phone Number: " << m_phone_number << std::endl
				<< std::setw(18) << std::left << "Darkest secret: " << m_darkest_secret << std::endl;
}

static std::string F_PrintString(std::string str) {
	if (str.length() > 10)
		return (str.substr(0, 9) + ".");
	return (str);
}

void	Contact::F_PrintContact(void) {
	char c = '|';

	std::cout	<< c << std::setw(10) << std::right << m_index 
				<< c << std::setw(10) << std::right << F_PrintString(m_first_name)
				<< c << std::setw(10) << std::right << F_PrintString(m_last_name)
				<< c << std::setw(10) << std::right << F_PrintString(m_nickname)
				<< c << std::endl;
	// std::cout << m_index << c << m_first_name << c << m_last_name << c<< m_nickname << std::endl;
}