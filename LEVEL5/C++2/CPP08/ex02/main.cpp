#include <iostream>
#include <stdexcept>
#include "errors.hpp"
#include "utils.hpp"
#include "MutantStack.hpp"

int main(void)
{
    try
    {
        MutantStack instance;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (FAILURE);
    }
    return (SUCCESS);
}
