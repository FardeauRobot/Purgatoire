#include <iostream>
#include <iomanip>
#include <ctime>
#include <cctype>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <stdexcept>
#include "errors.hpp"
#include "utils.hpp"
#include "PmergeMe.hpp"

static bool isValidArg(const char *s)
{
    if (*s == '\0')
        return (false);
    for (size_t i = 0; s[i]; ++i)
        if (!std::isdigit(static_cast<unsigned char>(s[i])))
            return (false);
    errno = 0;
    long n = std::strtol(s, NULL, 10);
    return (errno != ERANGE && n > 0 && n <= INT_MAX);
}

static void printArgs(int argc, char **argv)
{
    std::cout << "Before:";
    for (int i = 1; i < argc; ++i)
        std::cout << " " << argv[i];
    std::cout << endofline;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Error" << std::endl;
        return (FAILURE);
    }

    for (int i = 1; i < argc; ++i)
    {
        if (!isValidArg(argv[i]))
        {
            std::cerr << "Error" << std::endl;
            return (FAILURE);
        }
    }

    try
    {
        PmergeMe instance;

        printArgs(argc, argv);
        instance.sortVector(argc, argv);
        instance.sortDeque(argc, argv);

        std::cout << "After: ";
        Print(instance.getVector());
        std::cout << std::fixed << std::setprecision(5);
        std::cout << "Time to process a range of " << argc - 1
                  << " elements with std::vector : " << instance.getVectorTimer() << " us" << endofline;
        std::cout << "Time to process a range of " << argc - 1
                  << " elements with std::deque : " << instance.getDequeTimer() << " us" << endofline;
    }

    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (FAILURE);
    }
    return (SUCCESS);
}
