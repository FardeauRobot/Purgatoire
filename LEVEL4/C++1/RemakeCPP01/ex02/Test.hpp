#ifndef TEST_HPP
# define TEST_HPP

#include <string>

class Test {
    private:
        std::string m_name;

    public:
        Test(void);
        Test(std::string name);
        Test(const Test &src);
        Test& operator= (const Test &other);
        ~Test();
};

#endif
