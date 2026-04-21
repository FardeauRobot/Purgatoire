#include "Sed.hpp"
#include "Errors.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>

int main(int argc, char **argv)
{
	try
	{
		if (argc != 4)
			F_ErrExit("Usage: ./sed <file> <to_replace> <replace_by>");

		Sed sed(argv[1], argv[2], argv[3]);

		std::ifstream infile(sed.F_GetInfile().c_str());
		if (!infile)
			F_ErrExit("Cannot open infile: " + sed.F_GetInfile());

		std::ofstream outfile(sed.F_GetOutfile().c_str(), std::ios::trunc);
		if (!outfile)
			F_ErrExit("Cannot open outfile: " + sed.F_GetOutfile());

		std::string line;
		while (std::getline(infile, line))
			outfile << sed.F_ReplaceAll(line) << '\n';
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (FAILURE);
	}
	return (SUCCESS);
}
