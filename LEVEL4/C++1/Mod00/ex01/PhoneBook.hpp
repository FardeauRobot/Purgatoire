#ifndef PHONEBOOK_HPP
#define PHONEBOOK_HPP

#include "Contact.hpp"

#define SUCCESS 0
#define FAILURE 1

class PhoneBook
{
	private:
		Contact m_Contact[8];
		int m_index;
		int m_stored;

		int 		F_GetIndex() const;
		void		F_BookSetIndex (int newindex);
		Contact 	*F_GetContact(int index) ;
		void		F_AddStored(void);

	public:
		PhoneBook();					   // DEFAULT CONSTRUCTOR
		PhoneBook(const PhoneBook &other); // COPY CONSTRUCTOR
		PhoneBook &operator=(const PhoneBook &other);
		~PhoneBook(); // DESTRUCTOR

		void		F_BookPrint(void);
		int			F_Search(void);
		int			F_Add(void);
		void 		F_FillAll(void);
};

#endif