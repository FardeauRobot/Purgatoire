#include <iostream>
#include <istream>
#include <netinet/in.h>
#include <stdexcept>
#include "Client.hpp"

using namespace std;

int main (void) {
	try {
		Client Client1("Client 1");
		string to_print;
		if (Client1.setParameters(AF_INET, INADDR_LOOPBACK, 8080) != true)
			throw runtime_error("Couldn't set params");
		if (Client1.tryConnect() == true)
			cout << "ON EST EN LIGNEEEEE " << endl;
		else
			throw runtime_error("Connection not established");
		while ( getline(cin, to_print)) {
			if (to_print == "EXIT")
				break;
			else
				Client1.sendMessage(to_print);
		}
	}
	catch (...) {
		cerr << "Fallen into the PIT" << endl;
	}
}