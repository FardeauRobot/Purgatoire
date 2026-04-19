#ifndef SED_HPP
# define SED_HPP

#include <string>
#include <iostream>

class Sed {
    private: 
        std::string m_infile;
        std::string m_to_replace;
        std::string m_replace_by;
        std::string m_outfile;

    public:
        Sed(const std::string filepath, const std::string replaced, const std::string replacing);
        Sed(const Sed& other);
        Sed& operator=(const Sed& other);
        ~Sed();

        void    F_SedContentPrint();
        std::string  F_GetInfile();
        void    F_SetOutfile();
        std::string F_GetOutfile();
};

#endif