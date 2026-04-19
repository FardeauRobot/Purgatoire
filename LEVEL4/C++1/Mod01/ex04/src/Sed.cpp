#include "Sed.hpp"

Sed::Sed(const std::string filepath, const std::string replaced, const std::string replacing) {
   m_infile = filepath; 
   m_to_replace = replaced; 
   m_replace_by = replacing; 
   m_outfile = filepath + ".replace";
}

Sed::~Sed() { }

void    Sed::F_SedContentPrint() {
    std::cout << "INFILE = " << m_infile << std::endl
                << "TO_REPLACE = " << m_to_replace << std::endl
                << "REPLACE_BY = " << m_replace_by << std::endl
                << "OUTFILE = " << m_outfile << std::endl;
}

std::string  Sed::F_GetInfile(void) {
   return (m_infile);
}

std::string Sed::F_GetOutfile(void) {
   return (m_outfile);
}