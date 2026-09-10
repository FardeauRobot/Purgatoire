#ifndef SCALARCONVERTER_HPP
# define SCALARCONVERTER_HPP

#include <string>

class ScalarConverter {
    private:
        ScalarConverter();
        ScalarConverter(const ScalarConverter &src);
        ScalarConverter &operator= (const ScalarConverter &other);
        ~ScalarConverter();

    public:
        static void convert(const std::string &literal);
};

#endif
