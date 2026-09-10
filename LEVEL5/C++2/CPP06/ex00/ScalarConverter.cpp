#include "ScalarConverter.hpp"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <climits>
#include <cctype>

// ============================================================================
//  ORTHODOX CANONICAL FORM
//  ScalarConverter only exists to host the static convert() method: it holds
//  no data and must never be instantiated, copied or assigned. The four
//  members are therefore private and left empty on purpose.
// ============================================================================

ScalarConverter::ScalarConverter() {}

ScalarConverter::ScalarConverter(const ScalarConverter &src) { (void)src; }

ScalarConverter &ScalarConverter::operator= (const ScalarConverter &other) {
    (void)other;
    return (*this);
}

ScalarConverter::~ScalarConverter() {}

// ============================================================================
//  INTERNAL HELPERS  (file-local, not part of the public interface)
// ============================================================================

enum LiteralType {
    TYPE_CHAR,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_PSEUDO,
    TYPE_INVALID
};

// ----------------------------------------------------------------------------
//  TODO #1  --  the heart of the exercise.
//  Inspect `literal` and return which LiteralType it represents.
//
//    TYPE_PSEUDO : "nan" "nanf" "inf" "inff" "+inf" "-inf" "+inff" "-inff"
//    TYPE_CHAR   : 3 characters,  'x'  (quote, one NON-digit char, quote)
//    TYPE_INT    : optional + / - sign, then digits only          ->  -42
//    TYPE_FLOAT  : an otherwise-double literal ending in one 'f'   ->  4.2f
//    TYPE_DOUBLE : optional sign, digits, exactly one '.', digits  ->  -4.2
//    TYPE_INVALID: anything that does not match the shapes above
//
//  Hints:
//    - std::string gives you .size(), .find(), operator[], .substr()
//    - std::isdigit(static_cast<unsigned char>(c))  from <cctype>
//    - check the pseudo-literals and the char form BEFORE the number forms
//    - a lone "." , "-" , "f" , "42." , "42f" ... are all TYPE_INVALID
// ----------------------------------------------------------------------------
static LiteralType detectType(const std::string &literal) {
    (void)literal;              // <- remove once you use the parameter
    return (TYPE_INVALID);      // <- replace with your logic
}

// Formats a floating value the way the subject wants it: always at least one
// decimal digit ("0" -> "0.0", "42" -> "42.0"), leaves "4.2" untouched.
static std::string formatDecimal(double value) {
    std::ostringstream oss;

    oss << value;
    std::string s = oss.str();
    if (s.find('.') == std::string::npos && s.find('e') == std::string::npos
        && s.find("inf") == std::string::npos && s.find("nan") == std::string::npos)
        s += ".0";
    return (s);
    // NOTE: for very large magnitudes std::ostream switches to scientific
    //       notation ("2.14748e+09"). The usual subject test values are fine;
    //       come back and tune precision here if you want the big ones nicer.
}

static void printPseudo(const std::string &literal) {
    const bool         negative = (!literal.empty() && literal[0] == '-');
    const bool         isNan = (literal.find("nan") != std::string::npos);
    const std::string  sign = negative ? "-" : "";

    std::cout << "char: impossible" << std::endl;
    std::cout << "int: impossible" << std::endl;
    if (isNan) {
        std::cout << "float: nanf" << std::endl;
        std::cout << "double: nan" << std::endl;
    } else {
        std::cout << "float: " << sign << "inff" << std::endl;
        std::cout << "double: " << sign << "inf" << std::endl;
    }
}

static void printInvalid() {
    std::cout << "char: impossible" << std::endl;
    std::cout << "int: impossible" << std::endl;
    std::cout << "float: impossible" << std::endl;
    std::cout << "double: impossible" << std::endl;
}

// ----------------------------------------------------------------------------
//  TODO #2  --  char output.
//  `value` is the already-parsed numeric value of the literal.
//    - out of the char range (0 .. 127 for plain ASCII)  -> "impossible"
//    - else not printable, std::isprint(static_cast<unsigned char>(c)) is 0
//                                                         -> "Non displayable"
//    - else                                               -> 'c'
//  Use static_cast<char>(value) for the final conversion. <cctype> is included.
// ----------------------------------------------------------------------------
static void printAsChar(double value) {
    (void)value;                                          // <- remove when used
    std::cout << "char: " << "impossible" << std::endl;   // <- replace
}

// ----------------------------------------------------------------------------
//  TODO #3  --  int output.
//    - `value` does not fit in an int  (INT_MIN / INT_MAX, from <climits>)
//                                            -> "impossible"
//    - else                                  -> static_cast<int>(value)
// ----------------------------------------------------------------------------
static void printAsInt(double value) {
    (void)value;                                         // <- remove when used
    std::cout << "int: " << "impossible" << std::endl;   // <- replace
}

static void printAsFloat(double value) {
    const float f = static_cast<float>(value);

    std::cout << "float: " << formatDecimal(static_cast<double>(f)) << "f" << std::endl;
}

static void printAsDouble(double value) {
    std::cout << "double: " << formatDecimal(value) << std::endl;
}

// ============================================================================
//  PUBLIC API
// ============================================================================

void ScalarConverter::convert(const std::string &literal) {
    const LiteralType type = detectType(literal);

    if (type == TYPE_PSEUDO) {
        printPseudo(literal);
        return;
    }
    if (type == TYPE_INVALID) {
        printInvalid();
        return;
    }

    double value = 0.0;
    if (type == TYPE_CHAR)
        value = static_cast<double>(literal[1]);
    else
        value = std::strtod(literal.c_str(), NULL);

    printAsChar(value);
    printAsInt(value);
    printAsFloat(value);
    printAsDouble(value);
}
