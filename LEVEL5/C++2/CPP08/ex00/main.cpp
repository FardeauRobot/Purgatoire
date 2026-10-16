#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <deque>
#include <iterator>
#include <cstdlib>
#include <ctime>
#include <exception>
#include "errors.hpp"
#include "utils.hpp"
#include "easyfind.hpp"

template<typename T>
void printContainer(const T &container) {
    for (typename T::const_iterator it = container.begin(); it != container.end(); ++it) {
        std::cout << *it << endofline;
    }
}

template<typename T>
void findAndPrint(T &container, int n) {
    try {
        typename T::iterator found = easyfind(container, n);
        std::cout << BOLD_GREEN << "FOUND " << *found << " AT POSITION "
                  << std::distance(container.begin(), found) << endofline;
    }
    catch (std::exception &e) {
        F_ErrMsg(e.what());
    }
}

template<typename T>
void testContainer(const std::string &name, T &container) {
    std::cout << BOLD_CYAN << "[" << name << "]" << endofline;

    std::cout << BOLD_WHITE << "SEARCHING 42 IN AN EMPTY CONTAINER" << endofline;
    findAndPrint(container, 42);

    for (size_t i = 0; i < 10; i++) {
        container.push_back(std::rand() % 1000);
    }
    printContainer(container);

    typename T::iterator third = container.begin();
    std::advance(third, 2);
    std::cout << BOLD_WHITE << "SEARCHING THE THIRD ELEMENT (" << *third << ")" << endofline;
    findAndPrint(container, *third);

    std::cout << BOLD_WHITE << "SEARCHING -1 (NEVER INSERTED)" << endofline;
    findAndPrint(container, -1);
}

int main(void)
{
    std::srand(std::time(NULL));
    std::vector<int> int_vec;
    std::list<int>   int_list;
    std::deque<int>  int_deque;

    testContainer("STD::VECTOR", int_vec);
    testContainer("STD::LIST", int_list);
    testContainer("STD::DEQUE", int_deque);
    return (SUCCESS);
}
