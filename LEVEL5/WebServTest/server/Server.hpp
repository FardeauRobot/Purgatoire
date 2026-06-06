#ifndef SERVER_HPP
# define SERVER_HPP

#include <string>

class Server {
    private:
        std::string m_name;

    public:
        Server(void);
        Server(std::string name);
        Server(const Server &src);
        Server& operator= (const Server &other);
        ~Server();
};

#endif
