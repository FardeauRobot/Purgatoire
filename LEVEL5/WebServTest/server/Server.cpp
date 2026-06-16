#include "Server.hpp"
#include "utils.hpp"
#include <iostream>
#include <cstring>

Server::Server(void) {
	std::cout << BOLD_CYAN << "Server Default constructor called" << RESET << std::endl;
}

Server::Server(std::string name) : m_name(name) {
	std::cout << BOLD_CYAN << "Server Name constructor called" << RESET << std::endl;
}

Server::Server(const Server &src) {
	std::cout << BOLD_BLUE << "Server Copy constructor called" << RESET << std::endl;
	*this = src;
}

Server &Server::operator=(const Server &other) {
	std::cout << BOLD_BLUE << "Server Copy assignment operator called" << RESET << std::endl;
	if (this != &other)
		m_name = other.m_name;
	return (*this);
}

Server::~Server() {
	std::cout << BOLD_RED << "Server Destructor called" << RESET << std::endl;
}
