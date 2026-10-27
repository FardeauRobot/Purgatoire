#include <iostream>
#include <ctime>
#include <stdexcept>
#include "errors.hpp"
#include "utils.hpp"
#include "PmergeMe.hpp"



int main(void)
{
    try
    {
        PmergeMe instance;

        Print(instance.getDeque());
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (FAILURE);
    }
    return (SUCCESS);
}
