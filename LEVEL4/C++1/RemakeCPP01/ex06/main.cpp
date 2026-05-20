#include "Harl.hpp"

int main(int argc, char **argv) {
    Harl O_Harl;

    if (argc != 2)
        return (1);
    O_Harl.complain(argv[1]);
    return (0);
}
