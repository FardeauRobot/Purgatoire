#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

class Socket {
    private:
        std::string     m_name;
        int             m_listen_fd;
        sockaddr_in     m_addr;

    public:
        Socket(void);
        Socket(std::string name);
        Socket(const Socket &src);
        Socket& operator= (const Socket &other);
        ~Socket();

        void setSocket(int port);
        int     getSocketFd(void);
};

#endif
