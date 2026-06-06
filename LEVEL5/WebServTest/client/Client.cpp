#include <iostream>
#include "Client.hpp"



Client::Client(void) {
    std::cout << "Default constructor called" << std::endl;
}

Client::Client(std::string name): m_name(name) {
    std::cout << "Name constructor called" << std::endl;
}

Client::Client(const Client &src) {
    std::cout << "Copy constructor called" << std::endl;
    *this = src;
}

Client& Client::operator= (const Client &other) {
    std::cout << "Copy assignment operator called" << std::endl;
    if (this != &other)
        m_name = other.m_name;
    return (*this);
}

Client::~Client() {
    std::cout << "Destructor called" << std::endl;
}

bool	Client::tryConnect(void) const {

}