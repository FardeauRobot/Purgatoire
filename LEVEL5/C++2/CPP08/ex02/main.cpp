#include <iostream>
#include <list>
#include <stack>
#include "errors.hpp"
#include "utils.hpp"
#include "MutantStack.hpp"

#if SUBJECT == 0

static void testMutantStack(void)
{
    std::cout << BOLD_CYAN << "INIT MUTANT STACK" << endofline;
    MutantStack<int> mstack;

    int nb1 = 5;
    int nb2 = 17;

    std::cout << BOLD_WHITE << "PUSHING 2 NBRS : " << nb1 << " THEN " << nb2 << endofline;
    mstack.push(nb1);
    mstack.push(nb2);

    std::cout << BOLD_GREEN << "SIZE OF THE MUTANT STACK : " << mstack.size() <<endofline;
    std::cout << BOLD_WHITE << "NB AT TOP : " << BOLD_YELLOW << mstack.top() << endofline;
    std::cout << BOLD_RED << "POPPING THE MUTANT STACK" << endofline;
    mstack.pop();
    std::cout << BOLD_WHITE << "NB AT TOP : " << BOLD_YELLOW << mstack.top() << endofline;

    std::cout << BOLD_GREEN << "SIZE OF THE MUTANT STACK : " << mstack.size() <<endofline;

    std::cout << BOLD_WHITE << "PUSHING 4 NBRS" << endofline;
    mstack.push(3);
    mstack.push(5);
    mstack.push(737);
    mstack.push(0);

    std::cout << BOLD_GREEN << "SIZE OF THE MUTANT STACK : " << mstack.size() <<endofline;

    // TESTS FOR ITERATORS
    MutantStack<int>::iterator it = mstack.begin();
    MutantStack<int>::iterator ite = mstack.end();
    ++it;
    --it;

    std::cout << BOLD_CYAN << UNDERLINE << "ITERATING THROUGH THE MUTANT STACK :" << endofline;
    while (it != ite) {
        std::cout << BOLD_MAGENTA << *it << endofline;
        ++it;
    }

    std::cout << BOLD_CYAN << "CREATING A REAL STACK FROM MUTANT STACK (SIZE " << mstack.size() << ")" << endofline;

    std::stack<int> s(mstack);
    std::cout << BOLD_RED << "END OF TESTS FOR MUTANT STACK" << endofline;
}

static void testList(void)
{
    std::cout << BOLD_CYAN << "INIT LIST" << endofline;
    std::list<int> lst;

    int nb1 = 5;
    int nb2 = 17;

    std::cout << BOLD_WHITE << "PUSHING 2 NBRS : " << nb1 << " THEN " << nb2 << endofline;
    lst.push_back(nb1);
    lst.push_back(nb2);

    std::cout << BOLD_GREEN << "SIZE OF THE LIST : " << lst.size() << endofline;
    std::cout << BOLD_WHITE << "NB AT BACK : " << BOLD_YELLOW << lst.back() << endofline;
    std::cout << BOLD_RED << "POPPING THE LIST" << endofline;
    lst.pop_back();
    std::cout << BOLD_WHITE << "NB AT BACK : " << BOLD_YELLOW << lst.back() << endofline;

    std::cout << BOLD_GREEN << "SIZE OF THE LIST : " << lst.size() << endofline;

    std::cout << BOLD_WHITE << "PUSHING 4 NBRS" << endofline;
    lst.push_back(3);
    lst.push_back(5);
    lst.push_back(737);
    lst.push_back(0);

    std::cout << BOLD_GREEN << "SIZE OF THE LIST : " << lst.size() << endofline;

    // TESTS FOR ITERATORS
    std::list<int>::iterator it = lst.begin();
    std::list<int>::iterator ite = lst.end();
    ++it;
    --it;
    std::cout << BOLD_CYAN << UNDERLINE << "ITERATING THROUGH THE LIST :" << endofline;
    while (it != ite) {
        std::cout << BOLD_MAGENTA << *it << endofline;
        ++it;
    }

    std::cout << BOLD_CYAN << "CREATING A COPY OF THE LIST" << endofline;

    std::list<int> l(lst);
    std::cout << BOLD_RED << "END OF TESTS FOR LIST" << endofline;
}

#if EXTRAS == 1

static void testExtras(void)
{
    std::cout << BOLD_CYAN << "--- Copy / reverse / const ---" << endofline;
    MutantStack<int> mstack;

    for (int i = 1; i <= 5; i++)
        mstack.push(i * 10);

    MutantStack<int> copy(mstack);
    copy.push(60);
    MutantStack<int> assigned;
    assigned = copy;

    std::cout << BOLD_WHITE << "original size = " << BOLD_GREEN << mstack.size() << endofline;
    std::cout << BOLD_WHITE << "copy size     = " << BOLD_GREEN << copy.size() << endofline;

    std::cout << BOLD_WHITE << "reverse       : " << RESET;
    for (MutantStack<int>::reverse_iterator rit = assigned.rbegin(); rit != assigned.rend(); ++rit)
        std::cout << *rit << " ";
    std::cout << std::endl;

    const MutantStack<int> &cref = assigned;
    std::cout << BOLD_WHITE << "const         : " << RESET;
    for (MutantStack<int>::const_iterator cit = cref.begin(); cit != cref.end(); ++cit)
        std::cout << *cit << " ";
    std::cout << std::endl;
}
#endif

int main(void)
{
    testMutantStack();
    testList();
#if EXTRAS == 1
    testExtras();
#endif
    return (SUCCESS);
}

#else
int main()
{
    MutantStack<int> mstack;
    mstack.push(5);
    mstack.push(17);
    std::cout << mstack.top() << std::endl;
    mstack.pop();
    std::cout << mstack.size() << std::endl;
    mstack.push(3);
    mstack.push(5);
    mstack.push(737);
    //[...]
    mstack.push(0);
    MutantStack<int>::iterator it = mstack.begin();
    MutantStack<int>::iterator ite = mstack.end();
    ++it;
    --it;
    while (it != ite)
    {
        std::cout << *it << std::endl;
        ++it;
    }
    std::stack<int> s(mstack);
    return 0;
}
#endif
