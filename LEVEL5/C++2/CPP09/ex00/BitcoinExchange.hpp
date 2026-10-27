#ifndef BITCOINEXCHANGE_HPP
# define BITCOINEXCHANGE_HPP

#include <map>
#include <string>

# define MAP std::map<std::string, double>



class BitcoinExchange {
        MAP _rates;

        void    processLine(std::string const &line) const;

    public:
        BitcoinExchange();
        BitcoinExchange(const BitcoinExchange &src);
        BitcoinExchange& operator= (const BitcoinExchange &other);
        ~BitcoinExchange();

        int     loadDatabase(void);
        void    printDatabase(void) const;

        int     processInput(std::string const &file);
};

#endif
