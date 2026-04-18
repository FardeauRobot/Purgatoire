#include "Sed.hpp"
#include <iostream>

int main (int argc, char **argv) {
    if (argc != 4)
    {
        std::cerr << "Error format : ./sed file to_replace replace_by" << std::endl;
        return (1);
    }
    Sed O_Content(argv[1], argv[2], argv[3]);
    O_Content.F_SedContentPrint();
}

