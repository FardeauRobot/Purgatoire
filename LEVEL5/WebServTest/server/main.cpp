#include <cerrno>
#include <cstring>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <sys/poll.h>
#include <unistd.h>

#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>

#include "Socket.hpp"
#include "Server.hpp"


using namespace std;

bool g_running = true;

void signal_sigint(int) {
	g_running = false;
	cerr << "SIGNAL RECEIVED" << endl;
}

void error(string error) { cerr << error << endl; }

void display(string print) { cout << print << endl; }

int main(void) {
	try {
		// CREATING A SERVER LISTENING ON ONLY ONE PORT
		// SETTING UP UTILS
		Server serv("WebServTest");
		Socket socket1("SocketTest");
		signal(SIGINT, signal_sigint);
		int ret = 0;

		socket1.setSocket(PORT);
		int listenfd = socket1.getSocketFd();

		// INITIATE THE POLLFD VECTOR
		vector<pollfd> poll_fds;

		// ADD THE LISTEN POLLFD
		pollfd poll_listen;
		poll_listen.fd = listenfd;
		poll_listen.events = POLLIN;
		poll_fds.push_back(poll_listen);

		char	buffer[STD_BUFFER];

		// SETUP IS NOW COMPLETE, CAN LISTEN ON THE GIVEN FD.
		while (g_running) {

			// RUNS POLL ACROSS ALL FDS TO CHECK FOR ACTIONS
			ret = poll(&poll_fds[0], poll_fds.size(), TIMEOUT);
			if (ret == -1 && g_running)
				throw runtime_error("Poll failed miserably");

			// WHAT DO WE DO FOR EACH POLL_FD
			for (unsigned long i = 0; i < poll_fds.size(); i++) {
				// IF POLLFD IS THE RECEIVER ONE
				if (poll_fds[i].fd == listenfd && poll_fds[i].revents & POLLIN) {
					int client_fd = accept(listenfd, NULL, NULL);

					pollfd new_client;
					new_client.fd = client_fd;
					new_client.events = POLLIN;
					poll_fds.push_back(new_client);

					cout << "ADDED NEW CLIENT AT FD " << client_fd << endl;
				}

				else if (poll_fds[i].revents & POLLIN) {
					ssize_t n = recv(poll_fds[i].fd, buffer, STD_BUFFER, 0);
					if (n > 0) {
						cout << "MESSAGE FROM FD" << poll_fds[i].fd << " = " << buffer << endl;
						memset(buffer, 0, STD_BUFFER);
					} else if ( n == 0) {
						cout << "LOST THE CLIENT AT FD " << poll_fds[i].fd << endl;
						poll_fds.erase(poll_fds.begin() + i);
					}
				} else if (poll_fds[i].revents & ( POLLHUP | POLLERR)) {
					cout << "SUPPRESSING POLL_FD " << poll_fds[i].fd << endl;
					poll_fds.erase(poll_fds.begin() + i);
				}

			}
		}
	}

	// ERROR MANAGEMENT
	catch (runtime_error &e) {
		error("RUNTIME ERROR");
	} catch (exception &e) {
		error("EXCEPTION");
	} catch (...) {
		cerr << "FELL INTO THE PIT" << endl;
	}
	return 0;
}
