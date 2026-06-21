#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

class Client {
  protected:
	std::string m_name;
	int m_sockfd;
	sockaddr_in m_addr;
	socklen_t m_addrlen;

  public:
	Client(void);
	Client(std::string m_name);
	Client(const Client &src);
	Client &operator=(const Client &other);
	~Client();

	bool tryConnect(void);
	bool setParameters(int domain, int s_addr, int port);
	void sendMessage(std::string message);
};

#endif
