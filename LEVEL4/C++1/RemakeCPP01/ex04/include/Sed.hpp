#ifndef SED_HPP
# define SED_HPP

# include <string>

class Sed
{
    private:
        std::string m_infile;
        std::string m_to_replace;
        std::string m_replace_by;
        std::string m_outfile;

    public:
        Sed(const std::string& filepath, const std::string& replaced, const std::string& replacing);
        Sed(const Sed& other);
        Sed& operator=(const Sed& other);
        ~Sed();

        const std::string&  F_GetInfile()    const;
        const std::string&  F_GetOutfile()   const;
        const std::string&  F_GetToReplace() const;
        const std::string&  F_GetReplaceBy() const;

        void                F_SedContentPrint() const;
        std::string         F_ReplaceAll(const std::string& line) const;
};

#endif
