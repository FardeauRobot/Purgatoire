#ifndef SED_HPP
# define SED_HPP

#include <string>
#include <iostream>

class Sed {
    private: 
        std::string m_filepath;
        std::string m_to_replace;
        std::string m_replace_by;

    public:
        Sed(const std::string filepath, const std::string replaced, const std::string replacing);
        Sed(const Sed& other);
        Sed& operator=(const Sed& other);
        ~Sed();

        void    F_SedContentPrint();
};

#endif