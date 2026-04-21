#ifndef HARL_HPP
# define HARL_HPP

#include <string>

#define MSG_DEBUG "Debugging"
#define MSG_INFO "Info"
#define MSG_WARNING "Warning"
#define MSG_ERROR "Harl says an Error"

class Harl {
	private:

		void debug(void);
		void info(void);
		void warning(void);
		void error(void);

	public:
		Harl(void);
		~Harl(void);

		void complain(std::string level);
};


#endif