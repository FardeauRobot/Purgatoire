#include "Sed.hpp"
#include <iostream>

Sed::Sed(const std::string& filepath, const std::string& replaced, const std::string& replacing)
    : m_infile(filepath),
      m_to_replace(replaced),
      m_replace_by(replacing),
      m_outfile(filepath + ".replace")
{
}

Sed::Sed(const Sed& other)
    : m_infile(other.m_infile),
      m_to_replace(other.m_to_replace),
      m_replace_by(other.m_replace_by),
      m_outfile(other.m_outfile)
{
}

Sed& Sed::operator=(const Sed& other)
{
    if (this != &other)
    {
        m_infile     = other.m_infile;
        m_to_replace = other.m_to_replace;
        m_replace_by = other.m_replace_by;
        m_outfile    = other.m_outfile;
    }
    return (*this);
}

Sed::~Sed() {}

const std::string& Sed::F_GetInfile()    const { return (m_infile); }
const std::string& Sed::F_GetOutfile()   const { return (m_outfile); }
const std::string& Sed::F_GetToReplace() const { return (m_to_replace); }
const std::string& Sed::F_GetReplaceBy() const { return (m_replace_by); }

void Sed::F_SedContentPrint() const
{
    std::cout << "INFILE = "     << m_infile     << std::endl
              << "TO_REPLACE = " << m_to_replace << std::endl
              << "REPLACE_BY = " << m_replace_by << std::endl
              << "OUTFILE = "    << m_outfile    << std::endl;
}

std::string Sed::F_ReplaceAll(const std::string& line) const
{
    if (m_to_replace.empty())
        return (line);

    std::string result;
    size_t      pos = 0;
    size_t      found;

    while ((found = line.find(m_to_replace, pos)) != std::string::npos)
    {
        result += line.substr(pos, found - pos);
        result += m_replace_by;
        pos = found + m_to_replace.length();
    }
    result += line.substr(pos);
    return (result);
}
