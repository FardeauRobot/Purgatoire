#include <cctype>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include "BitcoinExchange.hpp"
#include "errors.hpp"
#include "utils.hpp"

// TORS ---------- 
BitcoinExchange::BitcoinExchange() { }


BitcoinExchange::BitcoinExchange(const BitcoinExchange &src) { *this = src; }


BitcoinExchange& BitcoinExchange::operator= (const BitcoinExchange &other) {
    if (this != &other)
        _rates = other._rates;
    return (*this);
}


BitcoinExchange::~BitcoinExchange() { }


// HELPERS ----------
// FORMAT DATE
static bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

static bool isValidDate(std::string const &date) {
    static const int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (date.size() != 10 || date[4] != '-' || date[7] != '-')
        return false;
    for (std::string::size_type i = 0; i < date.size(); ++i)
        if (i != 4 && i != 7 && !std::isdigit(static_cast<unsigned char>(date[i])))
            return false;

    int year  = std::atoi(date.substr(0, 4).c_str());
    int month = std::atoi(date.substr(5, 2).c_str());
    int day   = std::atoi(date.substr(8, 2).c_str());

    if (month < 1 || month > 12 || day < 1)
        return false;
    int maxDay = daysInMonth[month - 1];
    if (month == 2 && isLeapYear(year))
        maxDay = 29;
    return day <= maxDay;
}

// FORMAT VALUE
static bool isDecimal(std::string const &str) {
    std::string::size_type i = 0;
    bool                   hasDigit = false;
    bool                   hasDot = false;

    if (i < str.size() && str[i] == '-')
        ++i;
    for (; i < str.size(); ++i) {
        if (std::isdigit(static_cast<unsigned char>(str[i])))
            hasDigit = true;
        else if (str[i] == '.' && !hasDot)
            hasDot = true;
        else
            return false;
    }
    return hasDigit;
}

static bool parseNumber(std::string const &str, double &out) {
    if (!isDecimal(str))
        return false;

    std::istringstream iss(str);

    iss >> out;
    return !iss.fail();
}


// METHODS ----------

// LOAD DATABASE (.CSV)
int     BitcoinExchange::loadDatabase(void) {

    // CREATION DE LA MAP DES DONNEES DU DATA.CSV
    std::ifstream   datasheet("data.csv");
    std::string     dataline;

    if (!datasheet)
        return F_ErrMsg("could not open data.csv");
    std::getline(datasheet, dataline);
    while (std::getline(datasheet, dataline)) {
        std::string::size_type coma = dataline.find(",");
        double                 rate;

        if (coma == std::string::npos || !parseNumber(dataline.substr(coma + 1), rate)) {
            F_ErrMsg("Wrong input => " + dataline);
            continue;
        }
        _rates.insert(std::make_pair(dataline.substr(0, coma), rate));
    }
    return (SUCCESS);
}

// PROCESSING INPUT FILE
void    BitcoinExchange::processLine(std::string const &line) const {
    std::string::size_type sep = line.find(" | ");
    double                 value;

    if (sep == std::string::npos || !isValidDate(line.substr(0, sep))
        || !parseNumber(line.substr(sep + 3), value)) {
        F_ErrMsg("bad input => " + line);
        return;
    }
    if (value < 0) {
        F_ErrMsg("not a positive number.");
        return;
    }
    if (value > 1000) {
        F_ErrMsg("too large a number.");
        return;
    }

    std::string         date = line.substr(0, sep);
    MAP::const_iterator it = _rates.upper_bound(date);

    if (it == _rates.begin()) {
        F_ErrMsg("no rate available before " + date);
        return;
    }
    --it;
    std::cout << date << " => " << value << " = " << value * it->second << std::endl;
}


int     BitcoinExchange::processInput(std::string const &file) {
    std::ifstream   input(file.c_str());
    std::string     line;

    if (!input)
        return F_ErrMsg("could not open file.");
    if (std::getline(input, line) && line != "date | value")
        processLine(line);
    while (std::getline(input, line))
        processLine(line);
    return (SUCCESS);
}
// UTILS ----------
void    BitcoinExchange::printDatabase(void) const {

    for (MAP::const_iterator it = _rates.begin(); it != _rates.end(); ++it)
        std::cout << BOLD_RED << it->first << " => " << it->second << std::endl;
    std::cout << _rates.size() << " entries" << std::endl;
}

