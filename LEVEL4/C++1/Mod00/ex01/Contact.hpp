#ifndef CONTACT_HPP
# define CONTACT_HPP

# include <string>

class Contact {
	private :
		std::string m_first_name;
		std::string m_last_name;
		std::string m_nickname;
		std::string m_phone_number;
		std::string m_darkest_secret;
		int m_index;

	public :
		Contact(); // DEFAULT CONSTRUCTOR
		Contact(const Contact& other); // COPY CONSTRUCTOR
		Contact& operator=(const Contact& other);
		~Contact();	// DESTRUCTOR

		void	F_ContactSetIndex(int index) {m_index = index;}
		void	F_SetupContact(int index);
		void	F_PrintContact(void);
		void	F_PrintSearch(void);
		void	F_FillContact();
		void	F_FillContact(std::string first_name, std::string last_name, std::string nickname, std::string phone_number, std::string secret, int index) ;
};

#endif