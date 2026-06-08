#include "Client.hpp"
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdexcept>

using namespace std;

Client::Client(void) { std::cout << "Default constructor called" << std::endl; }

Client::Client(std::string name) : m_name(name) {
  std::cout << "Name constructor called" << std::endl;

  m_addrlen = sizeof(sockaddr_in);
  memset(&m_addr, 0, m_addrlen);

  m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (m_sockfd == -1)
    throw runtime_error("Error socket client");
  else
    cout << m_name << " has created its socket" << endl;

}

Client::Client(const Client &src) {
  std::cout << "Copy constructor called" << std::endl;
  *this = src;
}

Client &Client::operator=(const Client &other) {
  std::cout << "Copy assignment operator called" << std::endl;
  if (this != &other)
    m_name = other.m_name;
  return (*this);
}

Client::~Client() { std::cout << "Destructor called" << std::endl; }

bool    Client::setParameters(int domain, int s_addr, int port) {
	m_addr.sin_family = domain;
	m_addr.sin_port = htons(port);
	m_addr.sin_addr.s_addr = htonl(s_addr);
	m_addrlen = sizeof(m_addr);
	return (true);
}

void	Client::sendMessage(std::string message) {
	if (send(m_sockfd, message.c_str(), message.size(), MSG_NOSIGNAL) == -1)
		throw runtime_error("Couldn't send message");
}

bool Client::tryConnect(void) {
	if (connect(m_sockfd, reinterpret_cast<struct sockaddr *>(&m_addr), m_addrlen) == 0)
		return (true);
	return (false);
}