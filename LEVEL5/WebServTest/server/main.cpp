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

#include "Server.hpp"

#define PORT 8080
#define BACK_LOG 128
#define TIMEOUT 1000

#define STD_BUFFER 4096

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

	// 	Server a("WebServTest");
	// 	signal(SIGINT, signal_sigint);

	// 	int ret = 0;
	// 	int yes = 1;

	// 	// ============= FUNCTIONS 01 ==================
	// 	// CREATION OF THE SOCKETS
	// 	int sockfd1 = socket(AF_INET, SOCK_STREAM, 0);
	// 	int sockfd2 = socket(AF_INET, SOCK_STREAM, 0);

	// 	// SOCKETS OPEN WITH THE FD TABLE
	// 	cout << "SOCKFD1 = " << sockfd1 << " AND SOCKFD2 = " << sockfd2 << endl;

	// 	// SETTING SOCKETS PARAMS
	// 	ret = setsockopt(sockfd1, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	// 	cout << "RET SETSOCKOPT = " << ret << endl;
	// 	if (ret != 0)
	// 		throw runtime_error("setsockopt failed");

	// 	// BINDING THE ADDR
	// 	//  CREATING THE SOCKET STRUCT
	// 	struct sockaddr_in addr;
	// 	socklen_t len_socket = sizeof(addr);
	// 	memset(&addr, 0, len_socket);

	// 	// AFFECTING TO IPV4
	// 	addr.sin_family = AF_INET;
	// 	// AFFECTING TO PORT
	// 	addr.sin_port = htons(PORT);
	// 	//
	// 	addr.sin_addr.s_addr = INADDR_ANY; // accept on all interfaces (0.0.0.0) ->
		                                   // Doesn't matter if VPN / Wifi etc etc

	// 	// BINDING THE SOCKET TO THE TCP STREAM
	// 	ret = bind(sockfd1, reinterpret_cast<struct sockaddr *>(&addr), len_socket);
	// 	if (ret != 0) {
	// 		cout << "BINDING FAILURE\n\n" // 		     << endl;
	// 		throw runtime_error("Binding failed.");
	// 	} else
	// 		cout << "BINDING SUCCESS\n\n"
	// 		     << endl;

	// 	// LISTENING TO THE SOCKET STREAM
	// 	ret = listen(sockfd1, BACK_LOG);
	// 	if (ret != 0)
	// 		throw runtime_error("Listening failed.");
	// 	else
	// 		cout << "LISTENING SUCCESS on port : " << PORT << endl;

	// 	// Passing NULL and 0 means "watch no fds, just sleep for the timeout".
	// 	// It's a placeholder until you add your real fds.
	// 	// The server will sit quietly, wake up every second, check g_running, and
	// 	// go back to sleep.

	// 	// 	int ret = poll(NULL, 0, 1000); // sleep up to 1 second, watching nothing
	// 	// yet
	// 	//	if (ret == -1 && g_running) 		throw // runtime_error("poll
	// 	// failed");

	// 	size_t ret_recv = 0;

	// 	struct pollfd fds[1];
	// 	char buffer[4096];
	// 	bzero(buffer, 4096);
	// 	fds[0].fd = sockfd1;
	// 	fds[0].events = POLLIN;

	// 	// LOOP WHILE SERVER IS RUNNING
	// 	while (g_running) {

	// 		// USE OF POLL : CHECKS IF FDS ARE READY
	// 		// 1 
	// 		ret = poll(fds, 1, 1000);
	// 		if (ret == -1 && g_running)
	// 			throw runtime_error("poll failed");

	// 		if (fds[0].revents & POLLIN) {
	// 			int client_fd = accept(sockfd1, NULL, NULL);
	// 			if (client_fd == -1)
	// 				throw runtime_error("accept failed");
	// 			cout << "Client connected on fd " << client_fd << endl;

	// 			ret_recv = recv(fds[0].fd, buffer, 4096, 0);
	// 			if (ret_recv > 0) {
	// 				cout << buffer << endl;
	// 			} else if (ret_recv == 0) {
	// 				cout << "NEED TO CLOSE" << endl;
	// 			} else if (ret_recv < 0) {
	// 				if (errno == EAGAIN || errno == EWOULDBLOCK)
	// 					return (1);
	// 			}
	// 			close(client_fd);
	// 		}
	// 		if (fds[0].revents & POLLHUP) {
	// 			cout << "Lost connection to the client"
	// 		}
	// 	}

	// 	// CLOSING THE SERVER
	// 	if (close(sockfd1))
	// 		throw runtime_error("Coulnd't close socket");
	// 	else
	// 		cout << "CLOSING SUCCESS" << endl;

	// CREATING A SERVER LISTENING ON ONLY ONE PORT
		// SETTING UP UTILS
		Server serv("WebServTest");
		signal(SIGINT, signal_sigint);
		int ret = 0;
		int yes = 1;

		// CREATING THE SOCKET FOR THE LISTENING FD
		int listenfd = socket(AF_INET, SOCK_STREAM, 0);
		if (listenfd < 0)	
			throw runtime_error("Listening socket didn't initialize properly");
		else
			cout << "LISTENING SOCKET " << listenfd << " IS OPERATIONNAL";

		// SETTING OPTIONS TO LISTENING SOCKET
		ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
		if (ret != 0)
			throw runtime_error("Listening socket didn't set properly");

		// CREATING THE ADRESS STRUCTURE
		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(PORT);
		addr.sin_addr.s_addr = INADDR_ANY;

		// BINDING THE LISTENING SOCKET TO ADDRESS STRUCT
		ret = bind(listenfd, reinterpret_cast<sockaddr *>(&addr), sizeof(sockaddr));
		if (ret != 0) {
			cout << "BINDING FAILURE\n\n" << endl;
			throw runtime_error("Binding failed.");
		} else {
			cout << "BINDING SUCCESS\n\n" << endl;
		}

		// LISTENING TO THE SOCKET STREAM
		ret = listen(listenfd, BACK_LOG);
		if (ret != 0)
			cout << "Listening on FD " << listenfd << " failed miserably" << endl;
		else
			cout << "LISTENING SUCCESS on port : " << PORT << endl;
		


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
