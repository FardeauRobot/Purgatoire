#ifndef PHONEBOOK_HPP
#define PHONEBOOK_HPP

#include "Contact.hpp"

class PhoneBook
{
	private:
		Contact m_Contact[8];
		int m_index;
		int m_stored;

	public:
		PhoneBook();					   // DEFAULT CONSTRUCTOR
		PhoneBook(const PhoneBook &other); // COPY CONSTRUCTOR
		PhoneBook &operator=(const PhoneBook &other);
		~PhoneBook(); // DESTRUCTOR

		int 		F_GetIndex() const { return (m_index); };
		void		F_BookSetIndex (int newindex) {m_index = newindex;};
		Contact 	*F_GetContact(int index) { return (&m_Contact[index]); }
		void		F_AddStored(void);
		void		F_BookPrint(void);
		void 		F_FillAll(void);
};

#endif