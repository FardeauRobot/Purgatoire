#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <sys/socket.h>
#include <string>

class Client {
    protected:
        std::string m_name;
        int m_sockfd;
        sockaddr m_addr;
        socklen_t addrlen;

    public:
        Client(void);
        Client(std::string m_name);
        Client(const Client &src);
        Client& operator= (const Client &other);
        ~Client();

        bool	tryConnect(void) const ;
};

#endif
