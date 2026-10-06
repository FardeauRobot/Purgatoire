#include <iostream>
#include <string>
#include <cctype>
#include "utils.hpp"
#include "iter.hpp"

template<typename T>
void printElem(const T &elem) {
    std::cout << "[" << elem << "] ";
}

template<typename T>
void increment(T &elem) {
    elem++;
}

void shout(std::string &str) {
    for (std::string::size_type i = 0; i < str.size(); i++)
        str[i] = std::toupper(static_cast<unsigned char>(str[i]));
}

int main(void) {

    title("int array: print, increment, print");
    int nums[] = {0, 1, 2, 3, 4};
    std::cout << BOLD_MAGENTA << "ORIGINAL: ";
    iter(nums, 5, printElem<int>);
    std::cout << endofline;

    iter(nums, 5, increment<int>);

    std::cout << BOLD_CYAN << "INCREMENTED: ";
    iter(nums, 5, printElem<int>);
    std::cout << endofline;

    title("const int array: read-only function");
    const int constNums[] = {42, 21, 84};
    iter(constNums, 3, printElem<int>);
    std::cout << std::endl;

    title("std::string array: non-template function");
    std::string words[] = {"hello", "from", "iter"};
    iter(words, 3, printElem<std::string>);
    std::cout << std::endl;
    iter(words, 3, shout);
    iter(words, 3, printElem<std::string>);
    std::cout << std::endl;

    title("const char array");
    const char letters[] = "abc";
    iter(letters, 3, printElem<char>);
    std::cout << std::endl;

    title("length 0: nothing printed");
    iter(nums, 0, printElem<int>);
    std::cout << GREEN << "(ok)" << endofline;

    return (0);
}
