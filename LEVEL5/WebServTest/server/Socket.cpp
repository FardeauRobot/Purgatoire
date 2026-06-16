#include <netinet/in.h>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <sys/socket.h>

#include "Socket.hpp"
#include "utils.hpp"

Socket::Socket(void) {
    std::cout << "Default constructor called" << std::endl;
}

Socket::Socket(std::string name): m_name(name) {
    std::cout << "Name constructor called" << std::endl;
}

Socket::Socket(const Socket &src) {
    std::cout << "Copy constructor called" << std::endl;
    *this = src;
}

Socket& Socket::operator= (const Socket &other) {
    std::cout << "Copy assignment operator called" << std::endl;
    if (this != &other)
        m_name = other.m_name;
    return (*this);
}

Socket::~Socket() {
    std::cout << "Destructor called" << std::endl;
}

void    Socket::setSocket(int port) {
    int ret = 0;
    int yes = 1;

    // ALLOCATING THE SOCKET FOR THE LISTENING FD
    m_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listen_fd < 0)	
        throw std::runtime_error("Listening socket didn't initialize properly");
    else
        std::cout << "LISTENING SOCKET " << m_listen_fd << " IS OPERATIONNAL";

    // SETTING OPTIONS TO LISTENING SOCKET
    ret = setsockopt(m_listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    if (ret != 0)
        throw std::runtime_error("Listening socket didn't set properly");

    // SETTING UP ADDRESS STRUCTURE
    memset(&m_addr, 0, sizeof(sockaddr_in));
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(port);
    m_addr.sin_addr.s_addr = INADDR_ANY;

    // BINDING THE LISTENING SOCKET TO ADDRESS STRUCT
    ret = bind(m_listen_fd, reinterpret_cast<sockaddr *>(&m_addr), sizeof(sockaddr));
    if (ret != 0) {
        std::cout << "BINDING FAILURE\n\n" << std::endl;
        throw std::runtime_error("Binding failed.");
    } else {
        std::cout << "BINDING SUCCESS\n\n" << std::endl;
    }

    // LISTENING TO THE SOCKET STREAM
    ret = listen(m_listen_fd, BACK_LOG);
    if (ret != 0)
        std::cout << "Listening on FD " << m_listen_fd << " failed miserably" << std::endl;
    else
        std::cout << "LISTENING SUCCESS on port : " << PORT << std::endl;
	
}

int     Socket::getSocketFd(void) {
    return (m_listen_fd);
}