#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include "Location.hpp"
#include "Socket.hpp"

class Server {
  private:
	std::string				m_name;
	std::vector<Location>	m_locations;
	std::vector<Socket>		m_sockets;
	// size_t					m_max_body_size; // TODO : Ajouter le cap body_size
	// HOW TO STORE ERROR_PAGES (MAP ?)

  public:
	Server(void);
	Server(std::string name);
	Server(const Server &src);
	Server &operator=(const Server &other);
	~Server();
};

#endif
