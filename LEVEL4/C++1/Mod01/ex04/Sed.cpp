#include "Sed.hpp"

Sed::Sed(const std::string filepath, const std::string replaced, const std::string replacing) {
   m_filepath = filepath; 
   m_to_replace = replaced; 
   m_replace_by = replacing; 
}

Sed::~Sed() { }

void    Sed::F_SedContentPrint() {
    std::cout << "PATH = " << m_filepath << std::endl
                << "TO_REPLACE = " << m_to_replace << std::endl
                << "REPLACE_BY = " << m_replace_by << std::endl;
}