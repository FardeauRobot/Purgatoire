#include <exception>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/socket.h>

#include "Server.hpp"

#define PORT 8080
#define BACK_LOG 128

using namespace std;

bool	g_running = true;

void signal_sigint(int) {
	g_running = false;
	cerr << "SIGNAL RECEIVED" << endl;
}

void	error(string error) {
	cerr << error << endl;
}

void	display(string print) {
	cout << print << endl;
}



int main(void) {
	try {

		Server a("WebServTest");
		signal(SIGINT, signal_sigint);

		int ret = 0;
		int yes = 1;

		// ============= FUNCTIONS 01 ==================
		// CREATION OF THE SOCKETS
		int sockfd1 = socket(AF_INET, SOCK_STREAM, 0);
		int sockfd2 = socket(AF_INET, SOCK_STREAM, 0);

		// SOCKETS OPEN WITH THE FD TABLE
		cout << "SOCKFD1 = " << sockfd1 << " AND SOCKFD2 = " << sockfd2 << endl;

		// SETTING SOCKETS PARAMS
		ret = setsockopt(sockfd1, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
		cout << "RET SETSOCKOPT = " << ret << endl;
		if (ret != 0)
		throw runtime_error("setsockopt failed");




		// BINDING THE ADDR
		//  CREATING THE SOCKET STRUCT
		struct sockaddr_in addr;
		socklen_t len_socket = sizeof(addr);

		// AFFECTING TO IPV4
		addr.sin_family = AF_INET;
		// AFFECTING TO PORT
		addr.sin_port = htons(PORT);
		//
		addr.sin_addr.s_addr = INADDR_ANY; // accept on all interfaces (0.0.0.0) ->
											// Doesn't matter if VPN / Wifi etc etc


		// BINDING THE SOCKET TO THE TCP STREAM
		ret = bind(sockfd1, reinterpret_cast<struct sockaddr *>(&addr), len_socket);
		display("RET BIND");
		if (ret != 0)
			throw runtime_error("Binding failed.");
		else
			cout << "BINDING SUCCESS\n\n" << endl;


		// LISTENING TO THE SOCKET STREAM
		ret = listen(sockfd1, BACK_LOG);
		if (ret != 0)
			throw runtime_error("Listening failed.");
		else
			cout << "LISTENING SUCCESS on port : " << PORT << endl;

		int test = 0;
		while (g_running == true)
		{
			cout << test++ << endl;
		}

		// CLOSING THE SERVER
		if (close(sockfd1))
			throw runtime_error("Coulnd't close socket");
		else
			cout << "CLOSING SUCCESS" << endl;

	}
	catch (exception &e) {
		error("EXCEPTION");
	}
	catch (...) {
		cerr << "FELL INTO THE PIT" << endl;
	}
	return 0;
}
