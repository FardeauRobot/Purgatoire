#ifndef BRAIN_HPP
# define BRAIN_HPP

#include <string>

class Brain {
    private:
        std::string _ideas[100];

    public:
        Brain(void);
        Brain(const Brain &src);
        Brain& operator= (const Brain &other);
        ~Brain();

    void fillBrain(void);
    void    printBrain(void) ;
};

#endif
