#include "Sed.hpp"
#include <iostream>
#include <fstream>

int main (int argc, char **argv)
{
    if (argc != 4)
    {
        std::cerr << "Error format : ./sed file to_replace replace_by" << std::endl;
        return (1);
    }

    Sed O_Content(argv[1], argv[2], argv[3]);
    // JUST CHECKING THE CONTENT
    O_Content.F_SedContentPrint();

    // INITALIZING IN/OUTPUT
    std::ifstream infile(O_Content.F_GetInfile().c_str());
    std::ofstream outfile(O_Content.F_GetOutfile().c_str());

}
