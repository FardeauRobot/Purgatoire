#ifndef CONTACT_HPP
# define CONTACT_HPP

# define IN_COMMAND  "Waiting for the next command : "
# define IN_FIRSTNAME "Insert first name : "
# define IN_LASTNAME "Insert last name : "
# define IN_NICKNAME "Insert nickname : "
# define IN_PHONE "Insert phone number: "
# define IN_SECRET "What's your darkest secret \\(^ 3^)/? "
# define IN_EMPTY "Empty input is not allowed, please try again."
# define NO_USER "No user existing at index: " 
# define REGISTERED "\nUser has been well registered!\n"
# define CHOSE_INDEX "\n Waiting for user index: "
# define MENU "Returning to main menu\n" 
# define ERR_CMD "Wrong command: " 

# define WIDTH_CHOSEN 20
# define WIDTH_INDEX 10
# define WIDTH_SETUP 25

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

		void	F_ContactSetIndex(int index);
		int		F_SetupContact(int index);
		void	F_PrintContact(void);
		void	F_PrintSearch(void);
		void	F_FillContact();
		void	F_FillContact(std::string first_name, std::string last_name, std::string nickname, std::string phone_number, std::string secret, int index) ;
};

#endif