#include <iostream>
#include <string>
#include <exception>
#include <vector>
#include <list>
#include <algorithm>
#include <climits>
#include <cstdlib>
#include <ctime>
#include "errors.hpp"
#include "utils.hpp"
#include "Span.hpp"

# define NB_TEST 10000

#if SUBJECT == 0

static void printSpans(const Span &span)
{
    try {
        int shortest = span.shortestSpan();
        std::cout << BOLD_WHITE << "SHORTEST SPAN : " << BOLD_GREEN << shortest << endofline;
    }
    catch (std::exception &e) {
        F_ErrMsg(std::string("shortestSpan: ") + e.what());
    }
    try {
        int longest = span.longestSpan();
        std::cout << BOLD_WHITE << "LONGEST SPAN  : " << BOLD_GREEN << longest << endofline;
    }
    catch (std::exception &e) {
        F_ErrMsg(std::string("longestSpan: ") + e.what());
    }
}

static void testTooFew(void)
{
    std::cout << BOLD_MAGENTA << UNDERLINE << "[TOO FEW ELEMENTS]" << endofline;
    Span span(5);

    std::cout << BOLD_WHITE << "EMPTY SPAN" << endofline;
    printSpans(span);

    span.addNumber(42);
    std::cout << BOLD_WHITE << "ONE ELEMENT (42)" << endofline;
    printSpans(span);
}

static void testSubjectValues(void)
{
    std::cout << BOLD_MAGENTA << UNDERLINE << "[SUBJECT VALUES : 6 3 17 9 11]" << endofline;
    Span span(5);

    span.addNumber(6);
    span.addNumber(3);
    span.addNumber(17);
    span.addNumber(9);
    span.addNumber(11);
    std::cout << BOLD_YELLOW << "EXPECTED : 2 THEN 14" << endofline;
    printSpans(span);
}

static void testFull(void)
{
    std::cout << BOLD_MAGENTA << UNDERLINE << "[ADDING TO A FULL SPAN]" << endofline;
    Span span(2);

    try {
        span.addNumber(1);
        span.addNumber(2);
        std::cout << BOLD_WHITE << "2 / 2 SLOTS USED, ADDING A THIRD ONE" << endofline;
        span.addNumber(3);
    }
    catch (std::exception &e) {
        F_ErrMsg(e.what());
    }
}

static void testNegatives(void)
{
    std::cout << BOLD_MAGENTA << UNDERLINE << "[NEGATIVE NUMBERS : -5 7 -20]" << endofline;
    Span span(3);

    span.addNumber(-5);
    span.addNumber(7);
    span.addNumber(-20);
    std::cout << BOLD_YELLOW << "EXPECTED : 12 THEN 27" << endofline;
    printSpans(span);
}

static void testRange(void)
{
    std::cout << BOLD_MAGENTA << UNDERLINE << "[FILLING " << NB_TEST << " NUMBERS IN ONE CALL]" << endofline;
    std::vector<int> numbers(NB_TEST);
    std::generate(numbers.begin(), numbers.end(), std::rand);

    Span span(NB_TEST);
    span.fillSpan(numbers.begin(), numbers.end());
    std::cout << BOLD_WHITE << "SIZE OF THE SPAN : " << BOLD_GREEN << span.size() << endofline;
    printSpans(span);

    std::cout << BOLD_MAGENTA << UNDERLINE << "[FILLING FROM A STD::LIST : 100 10 1000]" << endofline;
    std::list<int> lst;
    lst.push_back(100);
    lst.push_back(10);
    lst.push_back(1000);

    Span from_list(3);
    from_list.fillSpan(lst.begin(), lst.end());
    std::cout << BOLD_YELLOW << "EXPECTED : 90 THEN 990" << endofline;
    printSpans(from_list);

    std::cout << BOLD_MAGENTA << UNDERLINE << "[FILLING WITH A RANGE TOO BIG]" << endofline;
    Span small(5);
    small.addNumber(1);
    small.addNumber(2);
    lst.push_back(10000);
    std::cout << BOLD_WHITE << "3 SLOTS LEFT, FILLING " << lst.size() << " NUMBERS" << endofline;
    try {
        small.fillSpan(lst.begin(), lst.end());
    }
    catch (std::exception &e) {
        F_ErrMsg(e.what());
    }
    std::cout << BOLD_WHITE << "SIZE OF THE SPAN (UNCHANGED) : " << BOLD_GREEN << small.size() << endofline;
}

static void testOverflow(void)
{
    std::cout << BOLD_MAGENTA << UNDERLINE << "[INT_MIN AND INT_MAX]" << endofline;
    Span span(2);

    span.addNumber(INT_MIN);
    span.addNumber(INT_MAX);
    printSpans(span);
}

static void testCopy(void)
{
    std::cout << BOLD_MAGENTA << UNDERLINE << "[COPY AND ASSIGNMENT]" << endofline;
    Span original(4);
    original.addNumber(1);
    original.addNumber(5);

    Span copy(original);
    Span assigned;
    assigned = original;

    original.addNumber(6);
    std::cout << BOLD_WHITE << "ORIGINAL GOT A 6 AFTER THE COPIES" << endofline;
    std::cout << BOLD_WHITE << "ORIGINAL SIZE : " << BOLD_GREEN << original.size() << endofline;
    printSpans(original);
    std::cout << BOLD_WHITE << "COPY SIZE     : " << BOLD_GREEN << copy.size() << endofline;
    printSpans(copy);
    std::cout << BOLD_WHITE << "ASSIGNED SIZE : " << BOLD_GREEN << assigned.size() << endofline;
    printSpans(assigned);
}

int main(void)
{
    std::srand(std::time(NULL));
    testTooFew();
    testSubjectValues();
    testFull();
    testNegatives();
    testRange();
    testOverflow();
    testCopy();
    return (SUCCESS);
}

#else

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
    return (0);
}
#endif
