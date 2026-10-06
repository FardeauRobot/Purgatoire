#include <iostream>
#include <exception>
#include "errors.hpp"
#include "Array.hpp"
#include "utils.hpp"
#include <iostream>

#define NB_SLOTS 20
#define MAX_VAL 750

#if SUBJECT == 0
int main(int, char**) {
    std::cout << BOLD_MAGENTA << "ARRAYS OF STRING" << endofline;
    Array<std::string> string_empty;
    Array<std::string> string_filled(NB_SLOTS);


    std::cout << BOLD_BLUE << "DISPLAY FIRST AND LAST ELEMENT OF ARRAY" << endofline;
    string_filled[0] = "First element";
    std::cout << BOLD_WHITE << "SLOT 0 = " << string_filled[0] << endofline;
    string_filled[NB_SLOTS - 1] = "Last element";
    std::cout << BOLD_WHITE << "SLOT " << NB_SLOTS - 1 << "=" << string_filled[NB_SLOTS - 1] << endofline;

    try {
        std::cout << BOLD_BLUE << "DISPLAY string_empty[0]" << endofline;
        std::cout << BOLD_BLUE << string_empty[0] << endofline;
    }
    catch (std::exception &e) {
        F_ErrMsg(e.what());
    }
    try {
        std::cout << BOLD_BLUE << "DISPLAY string_filled[NB_SLOTS] || MAX SLOT = " << NB_SLOTS << endofline;
        string_filled[NB_SLOTS] = 20;
        std::cout << BOLD_RED << "SLOT NB_SLOTS = " << string_filled[NB_SLOTS] << endofline;
    } 
    catch (std::exception &e) {
        F_ErrMsg(e.what()); 
    }
    try {
        std::cout << BOLD_BLUE << "DISPLAY string_filled[-1] || MIN SLOT = 0" << endofline;
        string_filled[-1] = 20;
        std::cout << BOLD_RED << "SLOT -1 = " << string_filled[-1] << endofline;
    } 
    catch (std::exception &e) {
        F_ErrMsg(e.what()); 
    }

    std::cout << BOLD_MAGENTA << "\nARRAYS OF INT" << endofline;
    Array<int> number_empty;
    Array<int> number_filled(NB_SLOTS);

    std::cout << BOLD_BLUE << "DISPLAY FIRST AND LAST ELEMENT OF ARRAY" << endofline;
    number_filled[0] = 1;
    std::cout << BOLD_WHITE << "SLOT 0 = " << number_filled[0] << endofline;
    number_filled[NB_SLOTS - 1] = 20;
    std::cout << BOLD_WHITE << "SLOT NB_SLOTS - 1 = " << number_filled[NB_SLOTS - 1] << endofline;
    try {
        std::cout << BOLD_BLUE << "DISPLAY number_empty[0]" << endofline;
        std::cout << BOLD_BLUE << number_empty[0] << endofline;
    }
    catch (std::exception &e) {
        F_ErrMsg(e.what());
    }
    try {
        std::cout << BOLD_BLUE << "DISPLAY number_filled[NB_SLOTS] || MAX SLOT = " << NB_SLOTS << endofline;
        number_filled[NB_SLOTS] = 20;
        std::cout << BOLD_RED << "SLOT NB_SLOTS = " << number_filled[NB_SLOTS] << endofline;
    } 
    catch (std::exception &e) {
        F_ErrMsg(e.what()); 
    }
    try {
        std::cout << BOLD_BLUE << "DISPLAY number_filled[-1] || MIN SLOT = 0" << endofline;
        number_filled[-1] = 20;
        std::cout << BOLD_RED << "SLOT -1 = " << number_filled[-1] << endofline;
    } 
    catch (std::exception &e) {
        F_ErrMsg(e.what()); 
    }

}

#else
#include <cstdlib>
int main(int, char**)
{
    Array<int> numbers(MAX_VAL);
    int* mirror = new int[MAX_VAL];
    srand(time(NULL));
    for (int i = 0; i < MAX_VAL; i++)
    {
        const int value = rand();
        numbers[i] = value;
        mirror[i] = value;
    }
    //SCOPE
    {
        Array<int> tmp = numbers;
        Array<int> test(tmp);
    }

    for (int i = 0; i < MAX_VAL; i++)
    {
        if (mirror[i] != numbers[i])
        {
            std::cerr << "didn't save the same value!!" << std::endl;
            return 1;
        }
    }
    try
    {
        numbers[-2] = 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    try
    {
        numbers[MAX_VAL] = 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    for (int i = 0; i < MAX_VAL; i++)
    {
        numbers[i] = rand();
    }
    delete [] mirror;//
    return 0;
}
#endif
