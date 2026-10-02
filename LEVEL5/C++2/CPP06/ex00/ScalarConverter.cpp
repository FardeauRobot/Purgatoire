#include "ScalarConverter.hpp"

#include <cstddef>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <climits>
#include <cctype>
#include <cerrno>
#include <limits>
#include <string>
#include "utils.hpp"

// ScalarConverter::ScalarConverter() {}
//
// ScalarConverter::ScalarConverter(const ScalarConverter &src) { (void)src; }
//
// ScalarConverter &ScalarConverter::operator= (const ScalarConverter &other) {
//     (void)other;
//     return (*this);
// }
//
// ScalarConverter::~ScalarConverter() {}

enum LiteralType {
    TYPE_CHAR,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_INVALID
};

static LiteralType detectType(const std::string &literal) {
    int i = 0;
    int pos_dot = -1;
    int pos_f = -1;
    int digits_before = 0;
    int digits_after = 0;

    if (literal == "nanf" || literal == "inff" ||
        literal == "+inff" || literal == "-inff")
        return (TYPE_FLOAT);
    if (literal == "nan" || literal == "inf" ||
        literal == "+inf" || literal == "-inf")
        return (TYPE_DOUBLE);

    if (literal.size() == 3 && literal[0] == '\'' && literal[2] == '\'')
        return (TYPE_CHAR);

    if (literal.size() == 1 && !std::isdigit(static_cast<unsigned char>(literal[0])))
        return (TYPE_CHAR);

    if (literal[0] == '-' || literal[0] == '+')
        i++;

    if (literal.size() > 0 && literal[literal.size() - 1] == 'f')
        pos_f = static_cast<int>(literal.size()) - 1;

    while (literal[i])
    {
        if (literal[i] == '.') {
            if (pos_dot != -1)
                return (TYPE_INVALID);
            pos_dot = i;
        } else if (std::isdigit(static_cast<unsigned char>(literal[i]))) {
            if (pos_dot == -1)
                digits_before++;
            else
                digits_after++;
        } else if (i != pos_f) {
            return (TYPE_INVALID);
        }
        i++;
    }

    if (digits_before == 0)
        return (TYPE_INVALID);
    if (pos_f != -1 && pos_dot == -1)
        return (TYPE_INVALID);
    if (pos_dot != -1) {
        if (digits_after == 0)
            return (TYPE_INVALID);
        if (pos_f != -1)
            return (TYPE_FLOAT);
        return (TYPE_DOUBLE);
    }
    return (TYPE_INT);
}

static std::string formatDecimal(double value) {
    std::ostringstream oss;

    oss << value;
    std::string s = oss.str();
    if (s.find('.') == std::string::npos && s.find('e') == std::string::npos
        && s.find("inf") == std::string::npos && s.find("nan") == std::string::npos)
        s += ".0";
    return (s);
}

static void printInvalid() {
    std::cout << "char: impossible" << endofline;
    std::cout << "int: impossible" << endofline;
    std::cout << "float: impossible" << endofline;
    std::cout << "double: impossible" << endofline;
}

static void printChar(bool possible, char c) {
    if (!possible)
        std::cout << "char: impossible" << endofline;
    else if (!std::isprint(static_cast<unsigned char>(c)))
        std::cout << "char: Non displayable" << endofline;
    else
        std::cout << "char: '" << c << "'" << endofline;
}

static void printInt(bool possible, int i) {
    if (!possible)
        std::cout << "int: impossible" << endofline;
    else
        std::cout << "int: " << i << endofline;
}

static void printFloat(float f) {
    std::cout << "float: " << formatDecimal(static_cast<double>(f)) << "f" << endofline;
}

static void printDouble(double d) {
    std::cout << "double: " << formatDecimal(d) << endofline;
}


static float doubleToFloat(double d) {
    if (d > std::numeric_limits<float>::max())
        return (std::numeric_limits<float>::infinity());
    if (d < -std::numeric_limits<float>::max())
        return (-std::numeric_limits<float>::infinity());
    return (static_cast<float>(d));
}

static void fromChar(char c) {
    printChar(true, c);
    printInt(true, static_cast<int>(c));
    printFloat(static_cast<float>(c));
    printDouble(static_cast<double>(c));
}

static void fromInt(int i) {
    printChar(i >= 0 && i <= 127, static_cast<char>(i));
    printInt(true, i);
    printFloat(static_cast<float>(i));
    printDouble(static_cast<double>(i));
}


static void fromFloat(float f) {
    const double asDouble = static_cast<double>(f);
    const bool   charOk = (f >= 0 && f <= 127);
    const bool   intOk = (asDouble >= INT_MIN && asDouble <= INT_MAX);

    char c = 0;
    int  i = 0;

    if (charOk)
        c = static_cast<char>(f);
    if (intOk)
        i = static_cast<int>(f);

    printChar(charOk, c);
    printInt(intOk, i);
    printFloat(f);
    printDouble(asDouble);
}

static void fromDouble(double d) {
    const bool charOk = (d >= 0 && d <= 127);
    const bool intOk = (d >= static_cast<double>(INT_MIN) && d <= static_cast<double>(INT_MAX));

    char c = 0;
    int  i = 0;

    if (charOk)
        c = static_cast<char>(d);
    if (intOk)
        i = static_cast<int>(d);

    printChar(charOk, c);
    printInt(intOk, i);
    printFloat(doubleToFloat(d));
    printDouble(d);
}

void ScalarConverter::convert(const std::string &literal) {
    switch (detectType(literal)) {
        case TYPE_CHAR:
            if (literal.size() == 3)
                fromChar(literal[1]);
            else
                fromChar(literal[0]);
            break;
        case TYPE_INT: {

            errno = 0;
            const long l = std::strtol(literal.c_str(), NULL, 10);
            if (errno == ERANGE || l < static_cast<long>(INT_MIN) || l > static_cast<long>(INT_MAX))
                fromDouble(std::strtod(literal.c_str(), NULL));
            else
                fromInt(static_cast<int>(l));
            break;
        }
        case TYPE_FLOAT:
            fromFloat(doubleToFloat(std::strtod(literal.c_str(), NULL)));
            break;
        case TYPE_DOUBLE:
            fromDouble(std::strtod(literal.c_str(), NULL));
            break;
        case TYPE_INVALID:
            printInvalid();
            break;
    }
}
