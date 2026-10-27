#include <iostream>
#include <exception>
#include <vector>
#include <list>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "Span.hpp"
#include "errors.hpp"
#include "utils.hpp"

# if SUBJECT == 0

static void printSpans(const Span &sp)
{
    try {
        int shortest = sp.shortestSpan();
        int longest = sp.longestSpan();
        std::cout << BOLD_WHITE << "shortest: " << BOLD_YELLOW << shortest << endofline;
        std::cout << BOLD_WHITE << "longest:  " << BOLD_YELLOW << longest << endofline;
    }
    catch (std::exception &e) {
        F_ErrMsg(e.what());
    }
}

int main()
{
    std::cout << BOLD_GREEN << "[SUBJECT VALUES : EXPECTED 2 THEN 14]" << endofline;
    Span sp = Span(5);
    sp.addNumber(6);
    sp.addNumber(3);
    sp.addNumber(17);
    sp.addNumber(9);
    sp.addNumber(11);
    printSpans(sp);

    std::cout << BOLD_GREEN << "[ADDING TO A FULL SPAN]" << endofline;
    try {
        sp.addNumber(42);
    }
    catch (std::exception &e) {
        F_ErrMsg(e.what());
    }

    std::cout << BOLD_GREEN << "[ONE ELEMENT]" << endofline;
    Span one(1);
    one.addNumber(42);
    printSpans(one);

    std::cout << BOLD_GREEN << "[FILLING FROM A STD::LIST : EXPECTED 90 THEN 990]" << endofline;
    std::list<int> lst;
    lst.push_back(100);
    lst.push_back(10);
    lst.push_back(1000);

    Span fromList(3);
    fromList.fillSpan(lst.begin(), lst.end());
    printSpans(fromList);

    std::cout << BOLD_GREEN << "[FILLING 10000 NUMBERS]" << endofline;
    std::srand(std::time(NULL));
    std::vector<int> numbers(10000);
    std::generate(numbers.begin(), numbers.end(), std::rand);
    Span big(10000);
    big.fillSpan(numbers.begin(), numbers.end());
    std::cout << BOLD_WHITE << "size: " << BOLD_YELLOW << big.size() << endofline;
    printSpans(big);

    std::cout << BOLD_GREEN << "[FILLING A FULL SPAN]" << endofline;
    try {
        big.fillSpan(lst.begin(), lst.end());
    }
    catch (std::exception &e) {
        F_ErrMsg(e.what());
    }
    return 0;
}

# else
int main()                                  
  {
  Span sp = Span(5);
  sp.addNumber(6);
  sp.addNumber(3);
  sp.addNumber(17);
  sp.addNumber(9);
  sp.addNumber(11);
  std::cout << sp.shortestSpan() << std::endl;
  std::cout << sp.longestSpan() << std::endl;
  return 0;
  }
#endif
