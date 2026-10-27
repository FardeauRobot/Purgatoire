#include <exception>
#include <iostream>
#include "errors.hpp"
#include "utils.hpp"
#include "BitcoinExchange.hpp"


int main(int argc, char **argv) {

    BitcoinExchange Datacenter;

    if (argc != 2)
        return F_ErrMsg("could not open file.");

    try {

        if (Datacenter.loadDatabase() == FAILURE)
            return (FAILURE);
        if (DEBUG)
            Datacenter.printDatabase();
        return Datacenter.processInput(argv[1]);

    } catch (std::exception const &e) {

        std::cerr << e.what() << endofline;
        return (FAILURE);
    }
}
