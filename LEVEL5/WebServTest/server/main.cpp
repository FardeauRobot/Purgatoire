#include <cerrno>
#include <cstring>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unistd.h>

#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>

#include "Server.hpp"

#define PORT 8080
#define BACK_LOG 128

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
    memset(&addr, 0, len_socket);

    // AFFECTING TO IPV4
    addr.sin_family = AF_INET;
    // AFFECTING TO PORT
    addr.sin_port = htons(PORT);
    //
    addr.sin_addr.s_addr = INADDR_ANY; // accept on all interfaces (0.0.0.0) ->
                                       // Doesn't matter if VPN / Wifi etc etc

    // BINDING THE SOCKET TO THE TCP STREAM
    ret = bind(sockfd1, reinterpret_cast<struct sockaddr *>(&addr), len_socket);
    if (ret != 0) {
      cout << "BINDING FAILURE\n\n" << endl;
      throw runtime_error("Binding failed.");
    } else
      cout << "BINDING SUCCESS\n\n" << endl;

    // LISTENING TO THE SOCKET STREAM
    ret = listen(sockfd1, BACK_LOG);
    if (ret != 0)
      throw runtime_error("Listening failed.");
    else
      cout << "LISTENING SUCCESS on port : " << PORT << endl;

    // Passing NULL and 0 means "watch no fds, just sleep for the timeout".
    // It's a placeholder until you add your real fds.
    // The server will sit quietly, wake up every second, check g_running, and
    // go back to sleep.

    // 	int ret = poll(NULL, 0, 1000); // sleep up to 1 second, watching nothing yet 
		//	if (ret == -1 && g_running) 		throw // runtime_error("poll failed");


    size_t ret_recv = 0;

    struct pollfd fds[1];
    char buffer[4096];
		bzero(buffer, 4096);
    fds[0].fd = sockfd1;
    fds[0].events = POLLIN;

		

    while (g_running) {

      ret = poll(fds, 1, 1000);
      if (ret == -1 && g_running)
        throw runtime_error("poll failed");
      if (fds[0].revents & POLLIN) {
        int client_fd = accept(sockfd1, NULL, NULL);
        if (client_fd == -1)
          throw runtime_error("accept failed");
        cout << "Client connected on fd " << client_fd << endl;

        ret_recv = recv(fds[0].fd, buffer, 4096, 0);
        if (ret_recv > 0) {
          cout << buffer << endl;
        } else if (ret_recv == 0) {
          cout << "NEED TO CLOSE" << endl;
        } else if (ret_recv < 0) {
          if (errno == EAGAIN || errno == EWOULDBLOCK)
            return (1);
        }
        close(client_fd);
      }
    }


    // CLOSING THE SERVER
    if (close(sockfd1))
      throw runtime_error("Coulnd't close socket");
    else
      cout << "CLOSING SUCCESS" << endl;

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
